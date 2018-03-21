#include <mpi.h>
int main(int argc, char *argv[]) {
int rank, buffer;
MPI_Status status;
MPI_Init (&argc, &argv);
MPI_Comm_rank (MPI_COMM_WORLD, &rank); if (rank == 0) {
buffer = 123;
MPI_Send (&buffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); }
else if (rank == 1) {
MPI_Recv (&buffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
 printf ("Received %d\n", buffer);
}
MPI_Finalize ();
 return 0;
}
