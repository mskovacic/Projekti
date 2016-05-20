#include <stdio.h>
#include <mpi.h>
#include <string.h>

int main(int argc, char** argv) {
    int my_rank;                /* Rank of process */
    int p;                      /* Number of processes */
    int source;                 /* Rank of sender */
    int dest;                   /* Rank of receiver */
    int tag = 50;               /* Tag for messages */
    char message[100];  /* Storage for the message */
    char name[32];      /* Processor name */
    int name_len;
    MPI_Status status;  /* Return status for receive */

    printf("start\n");
    MPI_Init(&argc, &argv);
    printf("MPI_Init OK\n");
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Get_processor_name(name, &name_len);
    if (my_rank != 0) {
        sprintf(message, "Greetings from process %d at %s!", my_rank, name);
        dest = 0;
        MPI_Send(message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    } else {
        for (source = 1; source < p; source++) {
            MPI_Recv(message, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
            printf("%s\n", message);
        }
    }

    MPI_Finalize();
    return 0;
}
