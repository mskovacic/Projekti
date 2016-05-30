#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define TRAZI_ZADATAK "daj zadatak\0"
#define NEMA_ZADATAKA "nema zadataka\0"
#define VISINA 6
#define SIRINA 7
#define BUFFER_SIZE VISINA*SIRINA
#define GORNJI_RED 0
#define SREDNJI_RED 1
#define DONJI_RED 2
#define RED_OZNAKE 3
#define POTEZ_CPU 0
#define POTEZ_HUMAN 1


void Inicijaliziraj_MPI(int *argc, char ***argv, int *mpi_rank, int *mpi_size, char *processor_name);
void Validiraj_MPI(int code);
int Ucitaj_dubinu_pretrazivanja();
void Nacrtaj_plocu(char *ploca);
void Nacrtaj_rub(char redak);
int Ucitaj_potez(char *broj_vrijednosti);
void Odigraj_potez(char igrac, int potez, char *ploca, char *broj_vrijednosti);
float Izracunaj_stanje_na_dubini(char *ploca, char *broj_vrijednosti, int dubina, int max_dubina);
int Izracunaj_stanje_ploce(char *ploca);
int Izracunaj_horizontalno_stanje(char *ploca);
int Izracunaj_vertikalno_stanje(char *ploca);
int Izracunaj_dijagonalno_gore_stanje(char *ploca);
int Izracunaj_dijagonalno_dolje_stanje(char *ploca);
int Iprobe(MPI_Status *status);
void Recv(void *buf, MPI_Status *status);
void Send(const void *buf, int dest);
void Isend(const void *buf, int dest, MPI_Request *request);
void Irecv(void *buf, MPI_Request *request);
void Wait(MPI_Request *request, MPI_Status *status);
void Zavrsi(int code, void *arg);

int main(int argc, char **argv) {
    int i, j, mpi_rank, mpi_size, igra_traje=1, potez, ucitan_potez, arg[2], stanje_ploce, dubina, ima_zadataka;
    char processor_name[MPI_MAX_PROCESSOR_NAME], ploca[VISINA*SIRINA], broj_vrijednosti[SIRINA], zadatak[BUFFER_SIZE], poruka[BUFFER_SIZE], *pom_ploca, *pom_broj_vrijednosti;
	MPI_Status status;

    Inicijaliziraj_MPI(&argc, &argv, &mpi_rank, &mpi_size, processor_name);
	
	if (mpi_rank == 0) {
			memset(ploca, 46, VISINA*SIRINA);
			memset(broj_vrijednosti, 0, SIRINA);
			//arg[0]=(int) ploca;
			//arg[1]=(int) broj_vrijednosti;
			//on_exit(Zavrsi, arg);
			dubina = Ucitaj_dubinu_pretrazivanja();
	}
	
	potez=POTEZ_CPU;
	while (igra_traje) {
		//igra_traje=0;
		
		if (mpi_rank == 0) {
			i=system("clear");
			switch (potez) {
				case POTEZ_CPU:
					printf("CPU na potezu\n");
					//TODO: DODATI LOGIKU OVDJE
					//POŠALJI PLOČU
					for (i=1; i<mpi_size; i++) {
						Send(ploca, i);
						Send(broj_vrijednosti, i);
					}
					/*
					for (i=1; i<mpi_size; i++) {
						Recv(poruka, &status);
						printf ("%s %d\n", poruka, status.MPI_SOURCE);
					}
					*/
					
					//GENERIRAJ ZADATKE
					memset(zadatak, 0, BUFFER_SIZE);
					zadatak[BUFFER_SIZE-1] = dubina-2;
					for(i=0; i<(int) pow(7, 2); i++) {
						for (j=0; j<2-1; j++) {
							if (zadatak[j] >= SIRINA) {
								zadatak[j] = 0;
								zadatak[j+1]++;
							}
						}
						zadatak[BUFFER_SIZE-2] = (char) i; 
						//POŠALJI ZADATKE
						if (Iprobe(&status)) {
							Recv(poruka, &status);
							if (strstr(poruka, TRAZI_ZADATAK)) {
								Send(zadatak, status.MPI_SOURCE);
							}
						}
						zadatak[0]++;
					}
					
					for (i=1; i<mpi_size; i++) {
						Recv(poruka, &status);
						if (strstr(poruka, TRAZI_ZADATAK)) {
							Send(NEMA_ZADATAKA, status.MPI_SOURCE);
						}
					}				
					
					
					//Nađi najbolji potez
					
					//Odigraj_potez(potez, ucitan_potez, ploca, broj_vrijednosti);
					sleep(5);
					potez = POTEZ_CPU;
					break;
				case POTEZ_HUMAN:
					printf("Igračev potez!\n");
					Nacrtaj_plocu(ploca);
					ucitan_potez = Ucitaj_potez(broj_vrijednosti);
					Odigraj_potez(potez, ucitan_potez, ploca, broj_vrijednosti);
					potez = POTEZ_HUMAN; //Treba ispraviti u produkciji
					break;
				default:
					perror("Došlo je do pogreške!");
			}
			stanje_ploce=Izracunaj_stanje_ploce(ploca);
			if (stanje_ploce != 0) {
				i=system("clear");
				Nacrtaj_plocu(ploca);
				//printf ("Stanje ploce: %d\n", stanje_ploce);
				if (stanje_ploce > 0) {
					printf ("Pobjeda računala!\n");
				} else {
					printf ("Pobjeda igrača!\n");
				}
				break;
			}
		} else {
			int nije_gotovo;
			float stanje_ploce;
			char pom_ploca[SIRINA*VISINA], pom_broj_vrijednosti[SIRINA];
			
			
			Recv(poruka, &status);
			memcpy(ploca, poruka, SIRINA*VISINA);
			Recv(poruka, &status);
			memcpy(broj_vrijednosti, poruka, SIRINA);
			//Nacrtaj_plocu(poruka);

			ima_zadataka = 1;
			while (ima_zadataka) {
				
				Send(TRAZI_ZADATAK, 0);
				Recv(poruka, &status);
				if (strstr(poruka, NEMA_ZADATAKA)) {
					ima_zadataka = 0;
					continue;
				}
				
				memcpy(pom_ploca, ploca, SIRINA*VISINA);
				memcpy(pom_broj_vrijednosti, broj_vrijednosti, SIRINA);
				
				dubina = poruka[BUFFER_SIZE-1];
				stanja = (double *) calloc(dubina, sizeof(double));
				//suma = (int *) calloc(dubina, sizeof(int));
				
				nije_gotovo = 1;
				while (nije_gotovo) {
					/*
					for(i=0; i < (int) pow(7, dubina); i++) {
						for (j=0; j<dubina-1; j++) {
							if (zadatak[j] >= SIRINA) {
								zadatak[j] = 0;
								zadatak[j+1]++;
							}
						}
						
						for (j=0; j < dubina; j++) {
							if (j % 2 == 0) {
								potez = POTEZ_CPU;
							} else {
								potez = POTEZ_HUMAN;
							}
							Odigraj_potez(potez, poruka[j], pom_ploca, pom_broj_vrijednosti);
							
							stanje = Izracunaj_stanje_ploce(pom_ploca);
							if (potez == POTEZ_HUMAN && stanje == -1 || potez == POTEZ_CPU && stanje == 1) {
								stanja[j] = stanje;
								if (j == 0) {
									nije_gotovo = 0;
									break;
								} else {
									stanja[j-1] = stanje;
								}
							} else {
								if (j == dubina - 1) {
									stanja[j] += stanje;
								}
							}
							
						}
						
						if (nije_gotovo == 0) {
							break;
						}
						//IZRAČUNA STANJE
						
						//najveća dubina
						stanje = Izracunaj_stanje_ploce(pom_ploca);
						suma[i % SIRINA] += stanje;
						if ((dubina-1) % 2 == 0) {
								potez = POTEZ_CPU;
							} else {
								potez = POTEZ_HUMAN;
							}
						
						zadatak[2]++;
					}
					//odigrani su svi potezi
					
					if (stanja[0] != 0) {
						break;
					} else {
						//izračunaj vjerojatnosti
						for (i = dubina-2; i>=0; i--) {
							if (stanja[i] != 0) {
								continue;
							}
							stanja[i]=(double) stanja[i-1]/SIRINA;
						}
					}
					*/
					dubina++;
				}
				
				//POŠALJI REZULTAT

				//Nacrtaj_plocu(pom_ploca);

			}

		}
	}
	
	printf("Igra je završena!\n");
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

int Ucitaj_dubinu_pretrazivanja() {
	int dubina;
	char ucitana_dubina[100];
	
	while(1){
		printf ("Unesi dubinu pretraživanja (>=4):");
		memset(ucitana_dubina, 0, 100);
		if (fgets (ucitana_dubina, 100, stdin) == NULL) {
			perror("\nIgra nasilno prekinuta! CTRL-D");
			exit(1);
		} else {
			dubina = atoi(ucitana_dubina);
			if (dubina >= 4) {
				break;
			}
			continue;
		}
	}
	return dubina;
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
	
	Nacrtaj_rub(RED_OZNAKE);
	Nacrtaj_rub(GORNJI_RED);
	for (i=VISINA-1; i>=0; i--) {
		printf ("%s", "│");
		for (j=0; j<SIRINA; j++) {
			printf (" %c %s", ploca[j+i*SIRINA], "│");
		}
		printf ("\n");
		if (i!= 0) Nacrtaj_rub(SREDNJI_RED);
	}
	Nacrtaj_rub(DONJI_RED);
}

void Nacrtaj_rub(char redak) {
	char red_element[10]="───", prazan_element[4]="   \0";
	
	//sprintf(red_element, "%s", "───");
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
		case RED_OZNAKE:
			printf ("  1%s2%s3%s4%s5%s6%s7\n", prazan_element, prazan_element, prazan_element, prazan_element, prazan_element, prazan_element);
			break;
		default:
			perror("Nacrtaj_rub error");
	}
}

int Ucitaj_potez(char *broj_vrijednosti) {
	int odigran_stupac;
	char ucitan_potez[100];
	
	while(1){
		printf("Upisi broj stupca za ubaciti (1-%d):", SIRINA);
		memset(ucitan_potez, 0, 100);
		if (fgets (ucitan_potez, 100, stdin) == NULL) {
			perror("\nIgra nasilno prekinuta! CTRL-D");
			exit(1);
		} else {
			odigran_stupac = atoi(ucitan_potez);
			if (odigran_stupac<=SIRINA && odigran_stupac>0) {
				odigran_stupac--;
				if (broj_vrijednosti[odigran_stupac]+1<=VISINA) {
					break;
				}
			}
			continue;
		}
	}
	return odigran_stupac;
}

void Odigraj_potez(char igrac, int potez, char *ploca, char *broj_vrijednosti) {
	char simbol;
	
	simbol = igrac == POTEZ_CPU ? 'X' : 'O';
	ploca[broj_vrijednosti[potez]*SIRINA+potez] = simbol;
	broj_vrijednosti[potez]++;
}

float Izracunaj_stanje_na_dubini(char *ploca, char *broj_vrijednosti, int dubina, int max_dubina) {
	int i;
	float suma, stanje;
	char igrac, pom_ploca[SIRINA*VISINA], pom_broj_vrijednosti[SIRINA];
	
	igrac = dubina % 2 == 0 ? POTEZ_CPU:POTEZ_HUMAN;
	
	for (i=0; i<SIRINA; i++) {
		memcpy(pom_ploca, ploca, SIRINA*VISINA);
		memcpy(pom_broj_vrijednosti, broj_vrijednosti, SIRINA);
		Odigraj_potez(igrac, i, pom_ploca, pom_broj_vrijednosti);
		if (dubina == max_dubina-1) {
			stanje = Izracunaj_stanje_ploce(pom_ploca);
		} else {
			stanje = Izracunaj_stanje_na_dubini(pom_ploca, pom_broj_vrijednosti, dubina+1, max_dubina);
		}
		if (igrac == POTEZ_HUMAN && stanje == -1 || igrac == POTEZ_CPU && stanje == 1) {
			return stanje;
		}
		suma += stanje;
	}
	if (suma == 0) {
		return Izracunaj_stanje_na_dubini(ploca, broj_vrijednosti, dubina, max_dubina+1);
	}
	return suma / SIRINA; 
	
}

int Izracunaj_stanje_ploce(char *ploca) {
	int code;
	
	code = Izracunaj_horizontalno_stanje(ploca);
	if (code != 0) return code;
	code = Izracunaj_vertikalno_stanje(ploca);
	if (code != 0) return code;
	code = Izracunaj_dijagonalno_gore_stanje(ploca);
	if (code != 0) return code;
	return Izracunaj_dijagonalno_dolje_stanje(ploca);
	
}

int Izracunaj_horizontalno_stanje(char *ploca) {
	int i, j, k, br_elemenata;
	char pomocno_polje[SIRINA>VISINA ? SIRINA:VISINA];
	
	for (i=0; i<VISINA; i++) {
		br_elemenata=0;
		memset(pomocno_polje, 0, SIRINA>VISINA ? SIRINA:VISINA);
		for (j=0; j<SIRINA; j++) {
			pomocno_polje[br_elemenata]=ploca[i*SIRINA+j];
			br_elemenata++;
		}
		if (strstr(pomocno_polje, "XXXX") != NULL) {
			return 1;
		} else if (strstr(pomocno_polje, "OOOO") != NULL) {
			return -1;
		}
		
	}
	return 0;
}

int Izracunaj_vertikalno_stanje(char *ploca) {
	int i, j, k, br_elemenata;
	char pomocno_polje[SIRINA>VISINA ? SIRINA:VISINA];
	
	for (i=0; i<SIRINA; i++) {
		br_elemenata=0;
		memset(pomocno_polje, 0, SIRINA>VISINA ? SIRINA:VISINA);
		for (j=0; j<VISINA; j++) {
			pomocno_polje[br_elemenata]=ploca[i+j*SIRINA];
			br_elemenata++;
		}
		if (strstr(pomocno_polje, "XXXX") != NULL) {
			return 1;
		} else if (strstr(pomocno_polje, "OOOO") != NULL) {
			return -1;
		}
		
	}
	return 0;
}

int Izracunaj_dijagonalno_gore_stanje(char *ploca) {
	int i, j, k, br_elemenata;
	char pomocno_polje[SIRINA>VISINA ? SIRINA:VISINA];
	
	for (i=VISINA-1; i>=0; i--) {
		for (j=0; j<SIRINA; j++) {
			if (i % VISINA == 0 || j % SIRINA == 0) {
				br_elemenata=0;
				memset(pomocno_polje, 0, SIRINA>VISINA ? SIRINA:VISINA);
				for (k=0; (i+k<VISINA) && j+k<SIRINA; k++) {
					pomocno_polje[br_elemenata]=ploca[i*SIRINA+j+k+k*SIRINA];
					br_elemenata++;
				}
				if (br_elemenata>=4) {
					if (strstr(pomocno_polje, "XXXX") != NULL) {
						return 1;
					} else if (strstr(pomocno_polje, "OOOO") != NULL) {
						return -1;
					}
				}
			}
		}
	}
	return 0;
}

int Izracunaj_dijagonalno_dolje_stanje(char *ploca) {
	int i, j, k, br_elemenata;
	char pomocno_polje[SIRINA>VISINA ? SIRINA:VISINA];
	
	for (i=VISINA-1; i>=0; i--) {
		for (j=SIRINA-1; j>=0; j--) {
			if (i == VISINA-1 || j % SIRINA == 0) {
				br_elemenata=0;
				memset(pomocno_polje, 0, SIRINA>VISINA ? SIRINA:VISINA);
				for (k=0; j+k<SIRINA && i-k>=0; k++) {
					pomocno_polje[br_elemenata]=ploca[i*SIRINA+j+k-k*SIRINA];
					br_elemenata++;
				}
				if (br_elemenata>=4) {
					if (strstr(pomocno_polje, "XXXX") != NULL) {
						return 1;
					} else if (strstr(pomocno_polje, "OOOO") != NULL) {
						return -1;
					}
				}
			}
		}
	}
	return 0;
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

void Zavrsi(int code, void *arg) {
	
	printf("Igra je završena!\n");
	free((char *) ((int *) arg)[0]);
	free((char *) ((int *) arg)[1]);
}
