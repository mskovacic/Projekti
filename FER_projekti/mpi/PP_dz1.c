#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
//#include <error.h>

#define LIJEVA 0
#define DESNA 1
#define JE_PRLJAVA 1
#define NIJE_PRLJAVA 0
#define KOD_MENE_JE 1
#define NIJE_KOD_MENE 0
#define TRAZENA 1
#define NIJE_TRAZENA 0
#define BUFFER_SIZE 10
#define PORUKA_TREBAM_LIJEVU "daj lijevu"
#define PORUKA_TREBAM_DESNU "daj desnu\0"
#define PORUKA_EVO_LIJEVA "evo lijeva"
#define PORUKA_EVO_DESNA "evo desna\0"
#define PORUKA_NEMAM_LIJEVU "nem lijeve"
#define PORUKA_NEMAM_DESNU "nem desne\0"


typedef struct _Vilica {
    unsigned char kod_mene;
    unsigned char prljava;
    unsigned char trazena;
    MPI_Status trazena_od;
} vilica;

void inicijaliziraj(int *argc, char ***argv, int *mpi_rank, int *mpi_size, char *processor_name);
void ispisi(int mpi_rank, char *str);
int lijevi_susjed_rank(int mpi_rank, int mpi_size);
int desni_susjed_rank(int mpi_rank, int mpi_size);
void misli(int mpi_rank, vilica *vilice);
void jedi(int mpi_rank, vilica *vilice);
void trazi_vilice(int mpi_rank, int mpi_size, vilica *vilice);
unsigned int generiraj_sekunde();
int odgovori_na_zahtjeve(vilica *vilice);
void obradi_poruku(char *buffer, MPI_Status *status, vilica *vilice);
void obradi_rezultat(int code);
void podijeli_vilice(int mpi_rank, int mpi_size, vilica *vilice);
int Iprobe(MPI_Status *status);
void Recv(void *buf, MPI_Status *status);
void Send(const void *buf, int dest);
void Isend(const void *buf, int dest, MPI_Request *request);
void Irecv(void *buf, MPI_Request *request);
void Wait(MPI_Request *request, MPI_Status *status);


int main(int argc, char **argv) {
    int i, mpi_rank, mpi_size;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    vilica vilice[2];

    inicijaliziraj(&argc, &argv, &mpi_rank, &mpi_size, processor_name);
    podijeli_vilice(mpi_rank, mpi_size, vilice);

    while (1) {
    	//printf("%d, %s, %d\n", mpi_rank, processor_name, rand());
	misli(mpi_rank, vilice);
	trazi_vilice(mpi_rank, mpi_size, vilice);
	jedi(mpi_rank, vilice);
	odgovori_na_zahtjeve(vilice);
    }

    MPI_Finalize();
    return 0;
}

void inicijaliziraj(int *argc, char ***argv, int *mpi_rank, int *mpi_size, char *processor_name) {
    int code, processor_name_len;

    memset(processor_name, 0, MPI_MAX_PROCESSOR_NAME);
    code = MPI_Init(argc, argv);
    obradi_rezultat(code);
    code = MPI_Comm_rank(MPI_COMM_WORLD, mpi_rank);
    obradi_rezultat(code);
    code = MPI_Comm_size(MPI_COMM_WORLD, mpi_size);
    obradi_rezultat(code);
    code = MPI_Get_processor_name(processor_name, &processor_name_len);
    obradi_rezultat(code);
    srand(time(NULL)/(*mpi_rank + 1) + *mpi_rank);
}

int lijevi_susjed_rank(int mpi_rank, int mpi_size) {
    return (mpi_rank + 1 + mpi_size) % mpi_size;
}

int desni_susjed_rank(int mpi_rank, int mpi_size) {
    return (mpi_rank - 1 + mpi_size) % mpi_size;
}

void ispisi(int mpi_rank, char *str) {
    printf("%*s\n", 25 * mpi_rank, str);
}

void misli(int mpi_rank, vilica *vilice) {
    unsigned int broj_sekundi = generiraj_sekunde();
    int flag, i, code, count;
    char buffer[BUFFER_SIZE];
    MPI_Status status;

    ispisi(mpi_rank, "mislim");
    for (i=0; i < broj_sekundi; i++) {
	odgovori_na_zahtjeve(vilice);
	sleep(1);
    }
}

void jedi(int mpi_rank, vilica *vilice) {
    unsigned int broj_sekundi = generiraj_sekunde();

    if (vilice[LIJEVA].kod_mene && vilice[DESNA].kod_mene) {
	ispisi(mpi_rank, "jedem");
	sleep(broj_sekundi);
	vilice[LIJEVA].prljava = NIJE_PRLJAVA;
	vilice[DESNA].prljava = NIJE_PRLJAVA;
    }
}

void trazi_vilice(int mpi_rank, int mpi_size, vilica *vilice) {
    MPI_Request request_lijeva, request_desna;
    MPI_Status status_lijeva, status_desna;
    char buffer[BUFFER_SIZE];
    int brojac;

    while (!vilice[LIJEVA].kod_mene || !vilice[DESNA].kod_mene) {
	if (!vilice[LIJEVA].kod_mene) {
	    ispisi(mpi_rank, "trazim lijevu vilicu");
	    Isend(PORUKA_TREBAM_LIJEVU, lijevi_susjed_rank(mpi_rank, mpi_size), &request_lijeva);
	}
	if (!vilice[DESNA].kod_mene) {
	    ispisi(mpi_rank, "trazim desnu vilicu");
	    Isend(PORUKA_TREBAM_DESNU, desni_susjed_rank(mpi_rank, mpi_size), &request_desna);
	}

	//Wait(&request_lijeva, &status_lijeva);
	//Wait(&request_desna, &status_desna);

	brojac=0;
	while (brojac < 2){
  	    brojac+=odgovori_na_zahtjeve(vilice);
	}
    }
}

int odgovori_na_zahtjeve(vilica *vilice) {
    int brojac=0;
    char buffer[BUFFER_SIZE];
    MPI_Status status;
    //MPI_Request request;

    while (Iprobe(&status)) {
 	brojac++;
	Recv(buffer, &status);
	if (strcmp(buffer, PORUKA_TREBAM_LIJEVU) == 0) {
	    /*
	    if (vilice[LIJEVA].kod_mene && !vilice[LIJEVA].prljava) {
		Send(PORUKA_EVO_LIJEVA, status.MPI_SOURCE);
		vilice[LIJEVA].kod_mene = NIJE_KOD_MENE;
	    } else {
		Send(PORUKA_NEMAM_LIJEVU, status.MPI_SOURCE);
	    }*/
	    vilice[LIJEVA].trazena = TRAZENA;
  	    vilice[LIJEVA].trazena_od = status;
	} else if (strcmp(buffer, PORUKA_TREBAM_DESNU) == 0) {
	    /*
	    if (vilice[DESNA].kod_mene && !vilice[DESNA].prljava) {
	    	Send(PORUKA_EVO_DESNA, status.MPI_SOURCE);
	    	vilice[DESNA].kod_mene = NIJE_KOD_MENE;
	    } else {
		Send(PORUKA_NEMAM_DESNU, status.MPI_SOURCE);
	    }*/
	    vilice[DESNA].trazena = TRAZENA;
	    vilice[DESNA].trazena_od = status;
	} else if (strcmp(buffer, PORUKA_EVO_LIJEVA) == 0) {
	    vilice[LIJEVA].kod_mene = KOD_MENE_JE;
	    vilice[LIJEVA].prljava = NIJE_PRLJAVA;
	    vilice[LIJEVA].trazena = NIJE_TRAZENA;
	} else if (strcmp(buffer, PORUKA_EVO_DESNA) == 0) {
	    vilice[DESNA].kod_mene = KOD_MENE_JE;
	    vilice[DESNA].prljava = NIJE_PRLJAVA;
	    vilice[DESNA].trazena = NIJE_TRAZENA;
	} else if (strcmp(buffer, PORUKA_NEMAM_LIJEVU) == 0) {
	} else if (strcmp(buffer, PORUKA_NEMAM_DESNU) == 0) {
	} else {
	    perror("Primljen buffer koji nebi smio biti!\n");
	}
    }

    if (vilice[LIJEVA].kod_mene && !vilice[LIJEVA].prljava && vilice[LIJEVA].trazena) {
	Send(PORUKA_EVO_LIJEVA, vilice[LIJEVA].trazena_od.MPI_SOURCE);
	vilice[LIJEVA].kod_mene = NIJE_KOD_MENE;
	vilice[LIJEVA].trazena = NIJE_TRAZENA;
    }
    if (vilice[DESNA].kod_mene && !vilice[DESNA].prljava && vilice[DESNA].trazena) {
	Send(PORUKA_EVO_DESNA, vilice[DESNA].trazena_od.MPI_SOURCE);
	vilice[DESNA].kod_mene = NIJE_KOD_MENE;
	vilice[DESNA].trazena = NIJE_TRAZENA;
    }

    return brojac;
}

int Iprobe(MPI_Status *status) {
    int code, resultlen, flag;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, status);
    obradi_rezultat(code);
    return flag;
}

void Recv(void *buf, MPI_Status *status) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Recv(buf, BUFFER_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
    obradi_rezultat(code);
}

void Send(const void *buf, int dest) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Send(buf, BUFFER_SIZE, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
    obradi_rezultat(code);
}

void Isend(const void *buf, int dest, MPI_Request *request) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Isend(buf, BUFFER_SIZE, MPI_CHAR, dest, 0, MPI_COMM_WORLD, request);
    obradi_rezultat(code);
}

void Irecv(void *buf, MPI_Request *request) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Irecv(buf, BUFFER_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, request);
    obradi_rezultat(code);
}

void Wait(MPI_Request *request, MPI_Status *status) {
    int code, resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    code = MPI_Wait(request, status);
    obradi_rezultat(code);
}

void podijeli_vilice(int mpi_rank, int mpi_size, vilica *vilice) {
    if (mpi_rank==0) {
	vilice[LIJEVA].prljava=JE_PRLJAVA;
	vilice[LIJEVA].kod_mene=KOD_MENE_JE;
	vilice[LIJEVA].trazena=NIJE_TRAZENA;
	vilice[DESNA].prljava=JE_PRLJAVA;
	vilice[DESNA].kod_mene=KOD_MENE_JE;
	vilice[DESNA].trazena=NIJE_TRAZENA;
    } else if (mpi_rank < mpi_size-1) {
	vilice[LIJEVA].prljava=JE_PRLJAVA;
	vilice[LIJEVA].kod_mene=KOD_MENE_JE;
	vilice[LIJEVA].trazena=NIJE_TRAZENA;
	vilice[DESNA].kod_mene=NIJE_KOD_MENE;
    } else {
	vilice[LIJEVA].kod_mene=NIJE_KOD_MENE;
	vilice[DESNA].kod_mene=NIJE_KOD_MENE;
    }
}

void obradi_rezultat(int code) {
    int resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    if (code != MPI_SUCCESS) {
	MPI_Error_string(code, error_string, &resultlen);
	perror(error_string);
    }
}

unsigned int generiraj_sekunde() {
    return rand() % 8 + 2;
}
