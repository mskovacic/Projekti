#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

int main(int argc, char **argv) {
	int **ploca, **vrijednosti_lijevo, **vrijednosti_gore;
	int rows, columns;
	int i, j;
	char *row, *token;
	
	printf("Input the table size (ROWS x COLUMNS):");
	scanf("%d x %d", &rows, &columns);
	
	ploca = (int **) calloc(sizeof(int *), rows);
	for (i=0; i < rows; i++)
	{
		ploca[i] = (int *) calloc(sizeof(int), columns);
	}
	
	//printf("Rows:%d, columns:%d\n", rows, columns);
	
	for (i=0; i < rows; i++) {
		for (j=0; j < columns; j++) {
			ploca[i][j] = rand() % 128;
		}
	}
	
	for (i=0; i < rows; i++) {
		for (j=0; j < columns; j++) {
			printf("%-4d", ploca[i][j]);
		}
		printf("\n");
	}
	
	printf("Upiši vrijednosti gore:");
	vrijednosti_gore = (int **) calloc(sizeof(int *), columns);
	for (i=0; i<columns; i++) {
		printf("column %d:", i);
		scanf("%s", row);
		token = strtok(row, " ");
		while( token != NULL ) 
	    {
			
			printf( " %s\n", token );
			token = strtok(NULL, s);
	    }
		//vrijednosti_gore[i]
	}
	
	
	//END
	for (i=0; i < rows; i++)
	{
		free(ploca[i]);
	}
	free(ploca);
	return 0;
}