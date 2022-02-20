#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

typedef struct cvor cvor;
struct cvor
{
	cvor *lijevi;
	cvor *desni;
	long vrijednost;
};

void makeBalTree(long *data, int left, int right, cvor **korijen)
{
	int middle = (left + right) / 2;
	if (left <= right) {
		*korijen=(cvor *) malloc(sizeof(cvor));
		(*korijen)->lijevi = NULL;
		(*korijen)->desni = NULL;
		(*korijen)->vrijednost = data[middle];
		makeBalTree(data, left, middle - 1, &(*korijen)->lijevi);
		makeBalTree(data, middle + 1, right, &(*korijen)->desni);
	}
}

void ispisiStablo(cvor *korijen, int brojac) 
{
	if (korijen != NULL) {
		printf("%*ld\n", brojac / 2, korijen->vrijednost);
		ispisiStablo(korijen->lijevi, brojac-1);
		ispisiStablo(korijen->desni, brojac+1);
	}
}

void makni_korijen(cvor *korijen) 
{
	if (korijen != NULL) {
		makni_korijen(korijen->lijevi);
		makni_korijen(korijen->desni);
		free(korijen);
	}
}

long* cmpfunc(const void *a, const void *b) {
	return (*(long*)a - *(long*)b);
}

int main(int argc, char **argv)
{
	long *niz;
	int brojac = 0;
	cvor *korijen;
	korijen = NULL;
	niz = NULL;

	printf("Upisi elemente stabla (ctrl-D za kraj):");
	do {
		niz = realloc(niz, (brojac+1)*sizeof(long));
		scanf("%ld",&niz[brojac]);
		brojac++;
	} while (niz[brojac-1]>=0);
	brojac--;
	//niz[brojac - 1] = NULL;

	qsort(niz, brojac - 1, sizeof(long), cmpfunc);
	/*
	for (i = 0; i < brojac - 1; i++) {
		printf("%ld ", niz[i]);
	}
	*/
	korijen = malloc(sizeof(cvor));
	korijen->vrijednost = 5;
	korijen->lijevi = NULL;
	korijen->desni = NULL;
	makeBalTree(niz, 0, brojac - 1, &korijen);
	ispisiStablo(korijen, 40);
	system("pause");
	free(niz);
	free(korijen);
	return 0;
}
