//#include "mpi.h"
//#include <stdio.h>
//#include <stdlib.h>
//
//#define BYTES_TO_MBITS(x) ((x) * 8 / (1024 * 1024)) // Convert bytes to Mbits
//
//int main(int argc, char *argv[]) {
//    int rank, size;
//    double t1, t2;
//    MPI_Init(&argc, &argv);
//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//    MPI_Comm_size(MPI_COMM_WORLD, &size);
//
//    // Varying message sizes
//    int message_sizes[] = {10000, 50000, 100000}; // Example message sizes
//    int num_sizes = sizeof(message_sizes) / sizeof(message_sizes[0]);
//
//    for (int i = 0; i < num_sizes; i++) {
//        int message_size = message_sizes[i];
//        double elapsed_time = 0.0;
//
//        // Allocate memory for message
//        char *message = (char *)malloc(message_size * sizeof(char));
//        if (message == NULL) {
//            printf("Memory allocation failed\n");
//            MPI_Abort(MPI_COMM_WORLD, 1);
//        }
//
//        // Start timer
//        t1 = MPI_Wtime();
//
//        // Perform communication
//        if (rank == 0) {
//            MPI_Send(message, message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
//            MPI_Recv(message, message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//        } else if (rank == 1) {
//            MPI_Recv(message, message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//            MPI_Send(message, message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
//        }
//
//        // Stop timer
//        t2 = MPI_Wtime();
//
//        // Calculate time taken for communication
//        elapsed_time = t2 - t1;
//
//        // Calculate throughput in Mbit/s
//        double throughput = BYTES_TO_MBITS(message_size) / elapsed_time;
//        // Print measured times to console
//        if (rank == 0) {
//            printf("Message size: %d bytes, Time taken: %1.6f seconds\n", message_size, elapsed_time);
//            printf("Throughput in [Mbit/s]: %f\n", throughput);
//        }
//
//        // Free memory
//        free(message);
//    }
//
//    MPI_Finalize();
//    return 0;
//}
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MESSAGE_SIZES_COUNT 4
#define REPETITIONS 5000

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int message_sizes[MESSAGE_SIZES_COUNT] = {1, 8, 64, 512}; // Include 1B message size
    double throughputs[MESSAGE_SIZES_COUNT] = {0};
    double latencies[MESSAGE_SIZES_COUNT] = {0};
    
    if (size < 2) {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    for (int i = 0; i < MESSAGE_SIZES_COUNT; i++) {
        int message_size = message_sizes[i];
        double total_time = 0;
        
        MPI_Barrier(MPI_COMM_WORLD); // synchronize before communication loop
        
        for (int j = 0; j < REPETITIONS; j++) {
            double start_time, end_time;
            char *buffer = (char *)malloc(message_size * sizeof(char));
            
            start_time = MPI_Wtime();
            if (rank == 0) {
                MPI_Send(buffer, message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
//                MPI_Recv(buffer, message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if (rank == 1) {
                MPI_Recv(buffer, message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//                MPI_Send(buffer, message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
            end_time = MPI_Wtime();
            
            total_time += end_time - start_time;
//            total_time += (end_time - start_time)/2;

            free(buffer);
        }
        
        MPI_Barrier(MPI_COMM_WORLD); // synchronize after communication loop
        
        double average_latency = (total_time / REPETITIONS) * 1000; // Latency in milliseconds
        
        double average_latency_by_process = average_latency/2;
        double throughput = ((double)message_size * REPETITIONS) / (total_time * 1000000 ); // Throughput in Mbit/s
        
        double throughput_by_process = throughput/2;
        
//        throughputs[i] = throughput;
//        latencies[i] = average_latency;
        
        throughputs[i] = throughput_by_process;
        latencies[i] = average_latency_by_process;
    }
    
    // Print results for each message size
    printf("Message Size (B)\tThroughput (Mbit/s)\tLatency (ms)\n");
    for (int i = 0; i < MESSAGE_SIZES_COUNT; i++) {
        printf("%d\t\t\t%f\t\t\t%f\n", message_sizes[i], throughputs[i], latencies[i]);
    }
    
    MPI_Finalize();
    return EXIT_SUCCESS;
}
