#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_REPEATS 5000

int main(int argc, char *argv[]) {
    int rank, size;
    double start_time, end_time, total_time;
    int message_sizes[] = {1, 100, 1000};
    int num_sizes = sizeof(message_sizes) / sizeof(message_sizes[0]);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        printf("This application requires exactly two processes.\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int partner_rank = (rank + 1) % 2;

    for (int i = 0; i < num_sizes; i++) {
        int msg_size = message_sizes[i];
        char *message = (char *)malloc(msg_size * sizeof(char));

        // Regular communication
        if (rank == 0) {
            start_time = MPI_Wtime();
            for (int j = 0; j < NUM_REPEATS; j++) {
                MPI_Send(message, msg_size, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD);
                MPI_Recv(message, msg_size, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            end_time = MPI_Wtime();
            total_time = end_time - start_time;
            printf("Message size: %d, Time taken for a single message (regular communication): %.10f\n", msg_size, total_time / (2 * NUM_REPEATS));
        } else if (rank == 1) {
            for (int j = 0; j < NUM_REPEATS; j++) {
                MPI_Recv(message, msg_size, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(message, msg_size, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD);
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

        // Buffered communication
        if (rank == 0) {
            start_time = MPI_Wtime();
            for (int j = 0; j < NUM_REPEATS; j++) {
                MPI_Bsend(message, msg_size, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD);
                MPI_Recv(message, msg_size, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            end_time = MPI_Wtime();
            total_time = end_time - start_time;
            printf("Message size: %d, Time taken for a single message (buffered communication): %.10f\n", msg_size, total_time / (2 * NUM_REPEATS));
        } else if (rank == 1) {
            for (int j = 0; j < NUM_REPEATS; j++) {
                MPI_Recv(message, msg_size, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Bsend(message, msg_size, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD);
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

        free(message);
    }

    printf("ended");
    MPI_Finalize();
    return EXIT_SUCCESS;
}
