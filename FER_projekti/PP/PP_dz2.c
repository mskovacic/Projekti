#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define VISINA 6
#define SIRINA 7
#define BUFFER_SIZE 10
#define GORNJI_RED 0
#define SREDNJI_RED 1
#define DONJI_RED 2
#define POTEZ_CPU 0
#define POTEZ_HUMAN 1


void Inicijaliziraj_MPI(int *argc, char ***argv, int *mpi_rank, int *mpi_size, char *processor_name);
void Validiraj_MPI(int code);
void Nacrtaj_plocu(char *ploca);
void Nacrtaj_rub(char redak);
int Iprobe(MPI_Status *status);
void Recv(void *buf, MPI_Status *status);
void Send(const void *buf, int dest);
void Isend(const void *buf, int dest, MPI_Request *request);
void Irecv(void *buf, MPI_Request *request);
void Wait(MPI_Request *request, MPI_Status *status);

int main(int argc, char **argv) {
    int i, j, mpi_rank, mpi_size, igra_traje=1, potez;
    char processor_name[MPI_MAX_PROCESSOR_NAME], *ploca, *broj_vrijednosti;

    Inicijaliziraj_MPI(&argc, &argv, &mpi_rank, &mpi_size, processor_name);
	ploca = (char *) calloc(VISINA*SIRINA, sizeof(char));
	broj_vrijednosti = (char *) calloc(SIRINA, sizeof(char));
	memset(ploca, 46, VISINA*SIRINA);
	memset(broj_vrijednosti, 48, SIRINA);
	
	
	if (mpi_rank == 0) {
		potez=POTEZ_HUMAN;
		while (igra_traje) {
			igra_traje=0;
			i=system("clear");
			switch (potez) {
				case POTEZ_CPU:
					printf("CPU na potezu\n");
					//TODO: DODATI LOGIKU OVDJE
					
					potez = POTEZ_HUMAN;
					break;
				case POTEZ_HUMAN:
					printf("Igračev potez!\n");
					Nacrtaj_plocu(ploca);
					//TODO: DODATI LOGIKU OVDJE
				
					potez = POTEZ_CPU;
					break;
					
				default:
					printf("Došlo je do pogreške!");
			}
			
		}
		
		printf("Igra je završena!\nPritisnite neku tipku za izlazak...\n");
	}
	Validiraj_MPI(MPI_Finalize());
    return 0;
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

void Validiraj_MPI(int code) {
    int resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    if (code != MPI_SUCCESS) {
	    MPI_Error_string(code, error_string, &resultlen);
	    perror(error_string);
    }
}

void Nacrtaj_plocu(char *ploca) {
	int i,j;
	
	Nacrtaj_rub(GORNJI_RED);
	for (i=0; i<VISINA; i++) {
		printf ("%s", "│");
		for (j=0; j<SIRINA; j++) {
			printf (" %c %s", ploca[j+i*SIRINA], "│");
		}
		printf ("\n");
		if (i!= VISINA-1) Nacrtaj_rub(SREDNJI_RED);
	}
	Nacrtaj_rub(DONJI_RED);
}

void Nacrtaj_rub(char redak) {
	char red_element[10];
	
	sprintf(red_element, "%s", "───");
	switch (redak) {
		case GORNJI_RED:
			printf ("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n", "┌", red_element, "┬", red_element, "┬", red_element, "┬", red_element, "┬", red_element, "┬", red_element, "┬", red_element, "┐");
			break;
		case SREDNJI_RED:
			printf ("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n", "├", red_element, "┼", red_element, "┼", red_element, "┼", red_element, "┼", red_element, "┼", red_element, "┼", red_element, "┤");
			break;
		case DONJI_RED:
			printf ("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n", "└", red_element, "┴", red_element, "┴", red_element, "┴", red_element, "┴", red_element, "┴", red_element, "┴", red_element, "┘");
			break;
		default:
			perror("Nacrtaj_rub error");
	}
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
