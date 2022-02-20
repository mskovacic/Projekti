#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include <menu.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define BUFFER_SIZE 10
#define POTEZ_CPU 0
#define POTEZ_HUMAN 1
#define COLOR_CPU 3
#define COLOR_HUMAN 4
#define COLOR_SELECT 5
#define CTRLD 	4

char *choices[] = {"1", "2", "3", "4", "5", "6", "7", (char *) NULL};

void Inicijaliziraj(int *argc, char ***argv, int *mpi_rank, int *mpi_size, char *processor_name);
void Inicijaliziraj_MPI(int *argc, char ***argv, int *mpi_rank, int *mpi_size, char *processor_name);
void Inicijaliziraj_sucelje();
WINDOW *Napravi_plocu();
WINDOW *Napravi_menu(int *n_choices, ITEM **items, MENU *menu);
void Validiraj_ncurses(int code);
void Validiraj_menu(int code);
void Validiraj_MPI(int code);
void Refresh();
void Wrefresh(WINDOW *prozor);
int Iprobe(MPI_Status *status);
void Recv(void *buf, MPI_Status *status);
void Send(const void *buf, int dest);
void Isend(const void *buf, int dest, MPI_Request *request);
void Irecv(void *buf, MPI_Request *request);
void Wait(MPI_Request *request, MPI_Status *status);
int Zavrsi(MENU *menu, ITEM **items, int n_choices);

int main(int argc, char **argv) {
    int i, j, mpi_rank, mpi_size, key, igra_traje=1, potez, n_choices;
    char processor_name[MPI_MAX_PROCESSOR_NAME], string[1000];
    WINDOW *ploca, *menu_window;
	ITEM **items;
	MENU *menu;

    Inicijaliziraj_MPI(&argc, &argv, &mpi_rank, &mpi_size, processor_name);
	if (mpi_rank == 0) {
		Inicijaliziraj_sucelje();
		Refresh();
		ploca = Napravi_plocu(); 
		menu_window = Napravi_menu(&n_choices, items, menu);

		/*
		wattron(ploca, COLOR_PAIR(3));
		//mvwaddch(ploca, 1, 1, ACS_BLOCK);
		mvwprintw(ploca, 1, 2, " ");
		wattroff(ploca, COLOR_PAIR(3));
		Wrefresh(ploca);
		
		wattron(ploca, COLOR_PAIR(4));
		//mvwaddch(ploca, 5, 5, ACS_DIAMOND);
		mvwprintw(ploca, 3, 5, " ");
		wattroff(ploca, COLOR_PAIR(4));
		Wrefresh(ploca);
		*/
		Refresh();
		potez=POTEZ_HUMAN;
		while (igra_traje) {
			igra_traje=0;
			switch (potez) {
				case POTEZ_CPU:
					//printw("CPU na potezu\n");
					//TODO: DODATI LOGIKU OVDJE
					potez = POTEZ_HUMAN;
					break;
				case POTEZ_HUMAN:
					//printw("Igracev potez!\nKoristi strelice za odabir stupca");
					//Refresh();
					//TODO: DODATI LOGIKU OVDJE
					while (getnstr(string, 1000)) {
						//move(0,0);
						//clear();
						//Refresh();
						mvprintw(0,0,"			pritisnut %s", string);
						/*
						switch (key) {
							case KEY_LEFT:
								menu_driver(menu, REQ_LEFT_ITEM);
								break;
							case KEY_RIGHT:
								menu_driver(menu, REQ_RIGHT_ITEM);
								break;
							case 10: //enter
							{
								ITEM *cur;
								cur=current_item(menu);
							}

							default:
								Refresh();
								continue;
						}
						*/
						Wrefresh(menu_window);
					}
					potez = POTEZ_CPU;
					break;
				default:
					printw("Došlo je do pogreške!");
					Refresh();
			}
			
		}
		
		printw("Igra je završena!\nPritisnite neku tipku za izlazak...");
		refresh();
		Zavrsi(menu, items, n_choices);
	}
	Validiraj_MPI(MPI_Finalize());
    return 0;
}

void Inicijaliziraj(int *argc, char ***argv, int *mpi_rank, int *mpi_size, char *processor_name) {
    
	Inicijaliziraj_MPI(argc, argv, mpi_rank, mpi_size, processor_name);
    Inicijaliziraj_sucelje();
}

void Inicijaliziraj_MPI(int *argc, char ***argv, int *mpi_rank, int *mpi_size, char *processor_name) {
	int processor_name_len;

    srand(time(NULL)/(*mpi_rank + 1) + *mpi_rank);
    memset(processor_name, 0, MPI_MAX_PROCESSOR_NAME);

    Validiraj_MPI(MPI_Init(argc, argv));
    Validiraj_MPI(MPI_Comm_rank(MPI_COMM_WORLD, mpi_rank));
    Validiraj_MPI(MPI_Comm_size(MPI_COMM_WORLD, mpi_size));
    Validiraj_MPI(MPI_Get_processor_name(processor_name, &processor_name_len));
}

void Inicijaliziraj_sucelje() {
	
	initscr();
    clear();
    //Validiraj_ncurses(noecho());
	//cbreak();
    //Validiraj_ncurses(keypad(stdscr, TRUE));
	Validiraj_ncurses(start_color());
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COLOR_CPU, COLOR_BLACK, COLOR_RED);
	init_pair(COLOR_HUMAN, COLOR_BLACK, COLOR_YELLOW);
	init_pair(COLOR_SELECT, COLOR_BLACK, COLOR_WHITE);
}

WINDOW *Napravi_plocu() {
	WINDOW *ploca;
	int i, height, width, v_border=8, h_border=9, board_width=7, board_height=6;

	height = v_border+board_height-1;
	width = h_border+board_width+board_width-1;
	ploca = newwin(height, width, 3, COLS/2-width/2);
	box(ploca, 0, 0);
	for (i=1; i<board_height; i++) {
		mvwhline(ploca, height/board_height*i, 1, ACS_HLINE, width-2);
	}
	for (i=1; i<board_width; i++) {
		mvwvline(ploca, 1, width/board_width*i, ACS_VLINE, height-2);
	}
	Wrefresh(ploca);
	return ploca;
}

WINDOW *Napravi_menu(int *n_choices, ITEM **items, MENU *menu) {
	int i, height, width;
	WINDOW *menu_window;
	
	*n_choices = ARRAY_SIZE(choices);
	items = (ITEM **) calloc(*n_choices, sizeof(ITEM *));
	for(i = 0; i < *n_choices; ++i)
        items[i] = new_item(choices[i], "");
	menu = new_menu((ITEM **) items);
	height = 3;
	width = 9+7+6;
	menu_window = newwin(height, width, 16, COLS/2-width/2);
	//Validiraj_ncurses(keypad(menu_window, TRUE));
	set_menu_win(menu, menu_window);
	set_menu_sub(menu, derwin(menu_window, height-1, width-1, 1, 1));
	set_menu_mark(menu, ">");
	set_menu_format(menu, 1, 7);
	menu_opts_off(menu, O_SHOWDESC);
	box(menu_window, 0, 0);
	post_menu(menu);
	Wrefresh(menu_window);
	return menu_window;
}

void Validiraj_ncurses(int code) {
	
	if (code != OK) {
		perror("Greška s ncurses funkcijom!");
	}
}

void Validiraj_menu(int code) {
	
	switch (code) {
		case E_OK:
			break;
		case E_BAD_ARGUMENT:
            perror("Routine detected an incorrect or out-of-range argument.");
			break;
		case E_BAD_STATE:
            perror("Routine was called from an initialization or termination function.");
			break;
		case E_NO_MATCH:
            perror("Character failed to match.");
			break;
		case E_NO_ROOM:
            perror("Menu is too large for its window.");
			break;
		case E_NOT_CONNECTED:
            perror("No items are connected to the menu.");
			break;
		case E_NOT_POSTED:
            perror("The menu has not been posted.");
			break;
		case E_NOT_SELECTABLE:
            perror("The designated item cannot be selected.");
			break;
		case E_POSTED:
            perror("The menu is already posted.");
			break;
		case E_REQUEST_DENIED:
            perror("The menu driver could not process the request.");
			break;
		case E_SYSTEM_ERROR:
            perror("System error occurred (see errno).");
			break;
		case E_UNKNOWN_COMMAND:
            perror("The menu driver code saw an unknown request code.");
			break;
		default:
			perror("Unknown menu error?!");
	}
}

void Validiraj_MPI(int code) {
    int resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    if (code != MPI_SUCCESS) {
	    MPI_Error_string(code, error_string, &resultlen);
	    perror(error_string);
    }
}

void Refresh() {
	int code;
	
	code = refresh();
	Validiraj_ncurses(code);
}

void Wrefresh(WINDOW *prozor) {
	int code;
	
	code = wrefresh(prozor);
	Validiraj_ncurses(code);
}

int Iprobe(MPI_Status *status) {
    int code, resultlen, flag;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, status);
    Validiraj_MPI(code);
    return flag;
}

void Recv(void *buf, MPI_Status *status) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Recv(buf, BUFFER_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
    Validiraj_MPI(code);
}

void Send(const void *buf, int dest) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Send(buf, BUFFER_SIZE, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
    Validiraj_MPI(code);
}

void Isend(const void *buf, int dest, MPI_Request *request) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Isend(buf, BUFFER_SIZE, MPI_CHAR, dest, 0, MPI_COMM_WORLD, request);
    Validiraj_MPI(code);
}

void Irecv(void *buf, MPI_Request *request) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Irecv(buf, BUFFER_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, request);
    Validiraj_MPI(code);
}

void Wait(MPI_Request *request, MPI_Status *status) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Wait(request, status);
    Validiraj_MPI(code);
}

int Zavrsi(MENU *menu, ITEM **items, int n_choices) {
	int i;
	
	Validiraj_menu(unpost_menu(menu));
    Validiraj_menu(free_menu(menu));
    for(i = 0; i < n_choices; ++i)
        Validiraj_menu(free_item(items[i]));
    Validiraj_ncurses(endwin());
	return 0;
}