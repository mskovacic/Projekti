#include <stdio.h>
#include <mpi.h>
#include <time.h>


void ograda(int rank, int size) {
	int i=0;
	MPI_Status status;
	char poruka[1];
	
	if (rank == 0) {
		for (i=1; i<size; i++) {
			MPI_Recv(poruka, 1, MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		}
		poruka[0]=2;
		for (i=1; i<size; i++) {
			MPI_Send(poruka, 1, MPI_CHAR, i, 1, MPI_COMM_WORLD);
		}
	} else {
		poruka[0]=1;
		MPI_Send(poruka, 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
		MPI_Recv(poruka, 1, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
	}
}


int main(int argc, char *argv[])
{
	char processor_name[100];
	int namelen, myid, numprocs;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Get_processor_name(processor_name, &namelen);
	
	printf("Proces %d na racunalu %s\n", myid, processor_name);
	ograda(myid, numprocs);
	//MPI_Barrier(MPI_COMM_WORLD);
	printf("OGRADA\n");
	//MPI_Barrier(MPI_COMM_WORLD);
	ograda(myid, numprocs);
	printf("Proces %d na racunalu %s\n", myid, processor_name);
	
	MPI_Finalize();
	return 0;
}