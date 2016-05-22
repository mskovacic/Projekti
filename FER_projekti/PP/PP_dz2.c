#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#define BUFFER_SIZE 10

void Inicijaliziraj(int *argc, char ***argv, int *mpi_rank, int *mpi_size, char *processor_name);
WINDOW *Napravi_plocu();
void RenderBoard(WINDOW *ploca);
void Obradi_rezultat(int code);
int Iprobe(MPI_Status *status);
void Recv(void *buf, MPI_Status *status);
void Send(const void *buf, int dest);
void Isend(const void *buf, int dest, MPI_Request *request);
void Irecv(void *buf, MPI_Request *request);
void Wait(MPI_Request *request, MPI_Status *status);

int main(int argc, char **argv) {
    int i, j, mpi_rank, mpi_size, key;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    WINDOW *ploca;

    Inicijaliziraj(&argc, &argv, &mpi_rank, &mpi_size, processor_name);
	//ploca = Napravi_plocu();
	refresh();
	ploca = Napravi_plocu(); 

	//mvwprintw(ploca, 1,1, "%c", ACS_BLOCK);
	//wmove(ploca, 1, 1);
	mvwaddch(ploca, 1, 1, ACS_DIAMOND);
	wrefresh(ploca);
	/*
    RenderBoard(ploca);

    while ((key=getch()) != KEY_F(1)) {
        move(0,0);
        clear();
        refresh();
        switch (key) {
            case KEY_UP:
                break;
            case KEY_DOWN:
                break;
            default:
                refresh();
        }
        RenderBoard(ploca);
    }
	*/
	getch();
    endwin();
    MPI_Finalize();
    return 0;
}

void Inicijaliziraj(int *argc, char ***argv, int *mpi_rank, int *mpi_size, char *processor_name) {
    int code, processor_name_len;

    srand(time(NULL)/(*mpi_rank + 1) + *mpi_rank);
    memset(processor_name, 0, MPI_MAX_PROCESSOR_NAME);

    code = MPI_Init(argc, argv);
    Obradi_rezultat(code);

    code = MPI_Comm_rank(MPI_COMM_WORLD, mpi_rank);
    Obradi_rezultat(code);

    code = MPI_Comm_size(MPI_COMM_WORLD, mpi_size);
    Obradi_rezultat(code);

    code = MPI_Get_processor_name(processor_name, &processor_name_len);
    Obradi_rezultat(code);

    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
}

WINDOW *Napravi_plocu() {
	WINDOW *ploca;
	int i, height, width;
	//LINES = 24
	//COLS = 80
	height = 8+6-1;
	width = 9+7+6;
	ploca = newwin(height, width, 2, COLS/2-width/2);
	box(ploca, 0, 0);
	for (i=1; i<7; i++) {
		mvwhline(ploca, height/6*i, 1, ACS_HLINE, width-2);
		mvwvline(ploca, 1, width/7*i, ACS_VLINE, height-2);
	}
	wrefresh(ploca);
	return ploca;
}

void RenderBoard(WINDOW *ploca) {
    int i=0,j=0;

    for (i=0; i<6; i++)
        for (j=0; j<7; j++) {
            box(ploca, 0, 0);
            wrefresh(ploca);
        }

}

int Iprobe(MPI_Status *status) {
    int code, resultlen, flag;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, status);
    Obradi_rezultat(code);
    return flag;
}

void Recv(void *buf, MPI_Status *status) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Recv(buf, BUFFER_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
    Obradi_rezultat(code);
}

void Send(const void *buf, int dest) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Send(buf, BUFFER_SIZE, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
    Obradi_rezultat(code);
}

void Isend(const void *buf, int dest, MPI_Request *request) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Isend(buf, BUFFER_SIZE, MPI_CHAR, dest, 0, MPI_COMM_WORLD, request);
    Obradi_rezultat(code);
}

void Irecv(void *buf, MPI_Request *request) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Irecv(buf, BUFFER_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, request);
    Obradi_rezultat(code);
}

void Wait(MPI_Request *request, MPI_Status *status) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Wait(request, status);
    Obradi_rezultat(code);
}

void Obradi_rezultat(int code) {
    int resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    if (code != MPI_SUCCESS) {
	    MPI_Error_string(code, error_string, &resultlen);
	    perror(error_string);
    }
}
