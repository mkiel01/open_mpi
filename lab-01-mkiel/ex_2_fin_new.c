#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MESSAGE_SIZES_COUNT 4
#define REPETITIONS 8000
#define BITS_PER_BYTE 8
#define MEGABIT 1000000.0

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    printf("Started\n");
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int message_sizes[MESSAGE_SIZES_COUNT] = {8, 64, 512, 4096};
    
    if (size < 2) {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    for (int i = 0; i < MESSAGE_SIZES_COUNT; i++) {
        int message_size = message_sizes[i];
        double total_time_regular = 0;
        
        char *buffer = (char *)malloc(message_size * sizeof(char));
        
        // Regular Communication
        for (int j = 0; j < REPETITIONS; j++) {
            double start_time, end_time;
            start_time = MPI_Wtime();
            if (rank == 0) {
                MPI_Send(buffer, message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
//                MPI_Recv(buffer, message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if (rank == 1) {
                MPI_Recv(buffer, message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//                MPI_Send(buffer, message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
            end_time = MPI_Wtime();
            total_time_regular += (end_time - start_time);
        }
        
        double average_latency_regular = total_time_regular / (2 * REPETITIONS); // Two processes
        double total_bytes_transferred = 2 * REPETITIONS * message_size;
        double throughput_mbps = (total_bytes_transferred * BITS_PER_BYTE) / (total_time_regular * MEGABIT);
        
        printf("Total Message size: %f - Total time taken: %f seconds\n", total_bytes_transferred, total_time_regular);
        printf("Message size: %d Regular Communication - Average latency: %f seconds\n",message_size, average_latency_regular);
        printf("Throughput: %f Mbit/s\n", throughput_mbps);
        
        free(buffer); // Free allocated memorys
    }
    
    MPI_Finalize();
}
