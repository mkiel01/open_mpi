#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);  /* starts MPI */
    MPI_Get_processor_name(hostname, &len);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  /* get current process id */
    MPI_Comm_size(MPI_COMM_WORLD, &size);  /* get number of processes */
    printf("Hello world from process %d of %d\n", rank, size);
    printf("Hostname: %s\n", hostname); // Corrected format specifier to %s
    MPI_Finalize();
    return 0;
}
