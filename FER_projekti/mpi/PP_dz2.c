#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
//#include <error.h>

#define BUFFER_SIZE 10

void Inicijaliziraj(int *argc, char ***argv, int *mpi_rank, int *mpi_size, char *processor_name);
void Obradi_rezultat(int code);
int Iprobe(MPI_Status *status);
void Recv(void *buf, MPI_Status *status);
void Send(const void *buf, int dest);
void Isend(const void *buf, int dest, MPI_Request *request);
void Irecv(void *buf, MPI_Request *request);
void Wait(MPI_Request *request, MPI_Status *status);


int main(int argc, char **argv) {
    int i, mpi_rank, mpi_size;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    Inicijaliziraj(&argc, &argv, &mpi_rank, &mpi_size, processor_name);

    printf("Uspješno pokrenut!\n");

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
