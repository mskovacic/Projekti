#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define TRAZI_ZADATAK 1
#define NEMA_ZADATAKA 0
#define IMA_ZADATAKA 1
#define REZULTAT 2
#define VISINA 6
#define SIRINA 7
#define BUFFER_SIZE VISINA*SIRINA
#define DUBINA_GLAVNOG 2
#define GORNJI_RED 0
#define SREDNJI_RED 1
#define DONJI_RED 2
#define RED_OZNAKE 3
#define POTEZ_CPU 0
#define POTEZ_HUMAN 1
#define BIT_TIP_PORUKE 0
#define BIT_DUBINA  1
#define BIT_RBR_PORUKE 2
#define BIT_POCETAK_ZADATKA 3
#define MASTER 0
#define SPOREDNI_SPREMNIK 100
#define STANJE_POBJEDA 1
#define STANJE_PORAZ -1
#define STANJE_QUO 0


char *Substring(char *string, unsigned int begin, unsigned int length);
void Napravi_poruku(char *poruka, char tip, char dubina, char rbr, char sadrzaj);
void Inicijaliziraj_MPI(int *argc, char ***argv, int *mpi_rank, int *mpi_size, char *processor_name);
int Ucitaj_dubinu_pretrazivanja();
void Nacrtaj_plocu(char *ploca);
void Nacrtaj_rub(char redak);
int Ucitaj_potez(char *broj_vrijednosti);
int Odigraj_potez(char igrac, int potez, char *ploca, char *broj_vrijednosti);
void Generiraj_zadatak(char *zadatak, int rbr);
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
void Validiraj_MPI(int code);

int main(int argc, char **argv) {
    int i, j, mpi_rank, mpi_size, igra_traje=1, potez, ucitan_potez, stanje_ploce, dubina=4, ima_zadataka, najbolje, rbr, code;
    char processor_name[MPI_MAX_PROCESSOR_NAME], ploca[VISINA*SIRINA], broj_vrijednosti[SIRINA], zadatak[BUFFER_SIZE-BIT_POCETAK_ZADATKA], poruka[BUFFER_SIZE], *substring;
	float stanje_cvora[SIRINA*SIRINA], seconds=0.0f;
	clock_t end, start;
	MPI_Status status;

    Inicijaliziraj_MPI(&argc, &argv, &mpi_rank, &mpi_size, processor_name);

	if (mpi_rank == MASTER) {
			memset(ploca, '.', VISINA*SIRINA);
			memset(broj_vrijednosti, '0', SIRINA);
			dubina = Ucitaj_dubinu_pretrazivanja();
	}

	potez=POTEZ_CPU;
	while (igra_traje) {
		//igra_traje=0;

		if (mpi_rank == MASTER) {
			i=system("clear");
			switch (potez) {
				case POTEZ_CPU:
					printf("CPU na potezu\n");
					//POŠALJI PLOČU
					start = clock();
					for (i=1; i<mpi_size; i++) {
						Send(ploca, i);
						Send(broj_vrijednosti, i);
					}
					
					//GENERIRAJ ZADATKE
					i = 0;
					while(i<(int) pow(SIRINA, DUBINA_GLAVNOG)) {
						Recv(poruka, &status);
						switch (poruka[BIT_TIP_PORUKE]) {
							case TRAZI_ZADATAK:
								Generiraj_zadatak(zadatak, i);
								memset(poruka, 0, BUFFER_SIZE);
								poruka[BIT_TIP_PORUKE]=IMA_ZADATAKA;
								poruka[BIT_DUBINA]= dubina-DUBINA_GLAVNOG;
								poruka[BIT_RBR_PORUKE] = (char) i;
								memcpy(poruka+BIT_POCETAK_ZADATKA, zadatak, BUFFER_SIZE-BIT_POCETAK_ZADATKA);
								Send(poruka, status.MPI_SOURCE);
								i++;
								break;
							case REZULTAT:
								rbr = poruka[BIT_RBR_PORUKE];
								substring = Substring(poruka, BIT_POCETAK_ZADATKA, BUFFER_SIZE-BIT_POCETAK_ZADATKA);
								stanje_cvora[rbr] = atof(substring);
								free(substring);
								break;
							default:
								perror("Primljen krivi tip poruke!");
						}
					}
					
					//POŠALJI DA NEMA VIŠE ZADATAKA
					for (i=1; i<mpi_size; i++) {
						Recv(poruka, &status);
						switch (poruka[BIT_TIP_PORUKE]) {
							case TRAZI_ZADATAK:
								memset(poruka, 0, BUFFER_SIZE);
								poruka[BIT_TIP_PORUKE]=NEMA_ZADATAKA;
								Send(poruka, status.MPI_SOURCE);
								break;
							case REZULTAT:
								rbr = poruka[BIT_RBR_PORUKE];
								substring = Substring(poruka, BIT_POCETAK_ZADATKA, BUFFER_SIZE-BIT_POCETAK_ZADATKA);
								stanje_cvora[rbr] = atof(substring);
								free(substring);
								i--;
								break;
							default:
								perror("Primljena nedozvoljena poruka!");
						}
					}

					//Nađi najbolji potez
					do {
						najbolje = -1;
						for (i=0; i<SIRINA; i++) {
							if (stanje_cvora[i] == 1) {
								najbolje = i;
								break;
							}
							for (j=0; j<SIRINA; j++) {
								if (stanje_cvora[SIRINA*j+i] == 1) {
									najbolje = i;
									break;
								}
								stanje_cvora[i]+=stanje_cvora[SIRINA*j+i];
							}
							if (najbolje != -1) {
								break;
							}
							printf("Stanje %d: %f\n", i, stanje_cvora[i]);
						}
						
						if (najbolje != -1) {
							code = Odigraj_potez(potez, najbolje, ploca, broj_vrijednosti);
							stanje_cvora[najbolje] = -1.0f;
							continue;							
						}

						najbolje = 0;
						for (i=0; i<SIRINA; i++) {
							if (stanje_cvora[i] > stanje_cvora[najbolje]) {
								najbolje = i;
							}
						}
						
						//DODATI PROVJERU DA SVI NISU -1
						
						code = Odigraj_potez(potez, najbolje, ploca, broj_vrijednosti);
						printf("%d code\n", code);
						sleep(5);
						stanje_cvora[najbolje] = -1.0f;
					} while (!code);
					end = clock();
					seconds = (float)(end - start) / CLOCKS_PER_SEC;
					potez = POTEZ_HUMAN;
					break;
				case POTEZ_HUMAN:
					printf("Potez računala je trajao: %f\n", seconds);
					printf("Igračev potez!\n");
					Nacrtaj_plocu(ploca);
					ucitan_potez = Ucitaj_potez(broj_vrijednosti);
					code = Odigraj_potez(potez, ucitan_potez, ploca, broj_vrijednosti);
					potez = POTEZ_CPU;
					break;
				default:
					perror("Došlo je do pogreške!");
			}
			stanje_ploce=Izracunaj_stanje_ploce(ploca);
			if (stanje_ploce != 0) {
				i=system("clear");
				Nacrtaj_plocu(ploca);
				if (stanje_ploce > 0) {
					printf ("Pobjeda računala!\n");
				} else {
					printf ("Pobjeda igrača!\n");
				}
				break;
			}
		} else {
			float stanje_ploce;
			char pom_ploca[SIRINA*VISINA], pom_broj_vrijednosti[SIRINA];
			
			//PRIMI PLOCU
			Recv(poruka, &status);
			memcpy(ploca, poruka, SIRINA*VISINA);
			
			//PRIMI BROJ VRIJEDNOSTI
			Recv(poruka, &status);
			memcpy(broj_vrijednosti, poruka, SIRINA);
			ima_zadataka = IMA_ZADATAKA;
			while (ima_zadataka) {
				memset(poruka, 0, BUFFER_SIZE);
				poruka[BIT_TIP_PORUKE] = TRAZI_ZADATAK;
				Send(poruka, MASTER);
				Recv(poruka, &status);
				
				if (poruka[BIT_TIP_PORUKE] == NEMA_ZADATAKA) {
					ima_zadataka = NEMA_ZADATAKA;
					continue;
				}
				dubina = poruka[BIT_DUBINA];
				rbr = poruka[BIT_RBR_PORUKE];
				
				i = 0;
				do {
					//NAPRAVI KOPIJU PLOCE
					memcpy(pom_ploca, ploca, SIRINA*VISINA);
					memcpy(pom_broj_vrijednosti, broj_vrijednosti, SIRINA);
					code = 1;
					
					for (j=0; j<dubina; j++) {
						potez = j % 2 == 0 ? POTEZ_CPU : POTEZ_HUMAN;
						code = Odigraj_potez(potez, poruka[BIT_POCETAK_ZADATKA+j], pom_ploca, pom_broj_vrijednosti);
						if (!code) {
							break;
						}
					}
					
					if (!code) {
						stanje_ploce = -1;
						break;
					}
					
					stanje_ploce = Izracunaj_stanje_na_dubini(pom_ploca, pom_broj_vrijednosti, 0, dubina+i);
					i++;
				} while (stanje_ploce == 0);
				memset(poruka, 0, BUFFER_SIZE);
				poruka[BIT_TIP_PORUKE] = REZULTAT;
				poruka[BIT_DUBINA] = dubina;
				poruka[BIT_RBR_PORUKE] = rbr;
				sprintf(poruka+BIT_POCETAK_ZADATKA, "%f", stanje_ploce);
				Send(poruka, MASTER);
			}
		}
	}

	printf("Igra je završena!\n");
	Validiraj_MPI(MPI_Finalize());
    return 0;
}

char *Substring(char *string, unsigned int begin, unsigned int length) {
	char *substring;

	substring = (char *) calloc(length+1, sizeof(char));
	memcpy(substring, string + begin, length);
	substring[length] = '\0';
	return substring;
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
	char ucitana_dubina[SPOREDNI_SPREMNIK];

	while(1){
		printf ("Unesi dubinu pretraživanja (>=4):");
		memset(ucitana_dubina, 0, SPOREDNI_SPREMNIK);
		if (fgets (ucitana_dubina, SPOREDNI_SPREMNIK, stdin) == NULL) {
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
	char red_element[10]="───\0", prazan_element[4]="   \0";

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
	char ucitan_potez[SPOREDNI_SPREMNIK];

	while (1) {
		printf("Upisi broj stupca za ubaciti (1-%d):", SIRINA);
		memset(ucitan_potez, 0, SPOREDNI_SPREMNIK);
		if (fgets (ucitan_potez, SPOREDNI_SPREMNIK, stdin) == NULL) {
			perror("\nIgra nasilno prekinuta! CTRL-D");
			exit(1);
		} else {
			odigran_stupac = atoi(ucitan_potez);
			if (odigran_stupac<=SIRINA && odigran_stupac>0) {
				odigran_stupac--;
				if (broj_vrijednosti[odigran_stupac]-48+1<=VISINA) {
					break;
				}
			}
			continue;
		}
	}
	return odigran_stupac;
}

int Odigraj_potez(char igrac, int potez, char *ploca, char *broj_vrijednosti) {
	char simbol, polje = broj_vrijednosti[potez]-48;

	if (potez >= SIRINA || potez < 0) {
		return -1;
	}

	//ploca je popunjena
	if (polje >= VISINA) {
		return 0;
	}
	
	simbol = igrac == POTEZ_CPU ? 'X' : 'O';
	ploca[polje*SIRINA+potez] = simbol;
	broj_vrijednosti[potez]++;
	return 1;
}

void Generiraj_zadatak(char *zadatak, int rbr) {
	int j;
	
	if (rbr == 0) {
		memset(zadatak, 0, BUFFER_SIZE-BIT_POCETAK_ZADATKA);	
	} else {
		zadatak[0]++;
		for (j=0; j<DUBINA_GLAVNOG; j++) {
			if (zadatak[j] >= SIRINA) {
				zadatak[j] = 0;
				zadatak[j+1]++;
			}
		}
	}
}

float Izracunaj_stanje_na_dubini(char *ploca, char *broj_vrijednosti, int dubina, int max_dubina) {
	int i;
	float suma=0, stanje;
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
		if ((igrac == POTEZ_HUMAN && stanje == STANJE_PORAZ) || (igrac == POTEZ_CPU && stanje == STANJE_POBJEDA)) {
			return stanje;
		}
		suma += stanje;
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
	int i, j, br_elemenata;
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
	int i, j, br_elemenata;
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
    int code, flag;

    code = MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, status);
    Validiraj_MPI(code);
    return flag;
}

void Recv(void *buf, MPI_Status *status) {
    int code;

    code = MPI_Recv(buf, BUFFER_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
    Validiraj_MPI(code);
}

void Send(const void *buf, int dest) {
    int code;

    code = MPI_Send(buf, BUFFER_SIZE, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
    Validiraj_MPI(code);
}

void Isend(const void *buf, int dest, MPI_Request *request) {
    int code;

    code = MPI_Isend(buf, BUFFER_SIZE, MPI_CHAR, dest, 0, MPI_COMM_WORLD, request);
    Validiraj_MPI(code);
}

void Irecv(void *buf, MPI_Request *request) {
    int code;

    code = MPI_Irecv(buf, BUFFER_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, request);
    Validiraj_MPI(code);
}

void Wait(MPI_Request *request, MPI_Status *status) {
    int code;

    code = MPI_Wait(request, status);
    Validiraj_MPI(code);
}

void Zavrsi(int code, void *arg) {

	printf("Igra je završena!\n");
	free((char *) ((int *) arg)[0]);
	free((char *) ((int *) arg)[1]);
}

void Validiraj_MPI(int code) {
    int resultlen;
    char error_string[MPI_MAX_ERROR_STRING];

    if (code != MPI_SUCCESS) {
	    MPI_Error_string(code, error_string, &resultlen);
	    perror(error_string);
    }
}
