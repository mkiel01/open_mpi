#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MESSAGE_SIZES_COUNT 5
#define REPETITIONS 8000

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int message_sizes[MESSAGE_SIZES_COUNT] = {8, 64, 512, 4096, 32768};
    
    if (size < 2) {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    for (int i = 0; i < MESSAGE_SIZES_COUNT; i++) {
        int message_size = message_sizes[i];
        double total_time_regular = 0;
        //        double total_time_buffered = 0;
        
        double start_time, end_time;
        
        char buffer = (char *)malloc(message_size * sizeof(char));
        
        
        // Regular Communication
        start_time = MPI_Wtime();
        for (int j = 0; j < REPETITIONS; j++) {
            if (rank == 0) {
                MPI_Send(&buffer, message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                //                    MPI_Recv(buffer, message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if (rank == 1) {
                MPI_Recv(&buffer, message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //                    MPI_Send(buffer, message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }
        end_time = MPI_Wtime();
        total_time_regular += (end_time - start_time);
        
    }
        
        
        double average_latency_regular = total_time_regular / (REPETITIONS);
        double average_latency_regular_by_process = average_latency_regular/2;
        
        printf("Message size: %d\n", message_size);
        printf("Regular Communication - Average latency: %f seconds\n", average_latency_regular_by_process);
        
    
    
    MPI_Finalize();
    
}
