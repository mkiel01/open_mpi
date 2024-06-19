#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <number_of_points>\n", argv[0]);
        return 1;
    }
    long npoints = atol(argv[1]);
    long circle_count = 0;
    double xcoordinate, ycoordinate;
    double start, end;
    double cpu_time_used;

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long num = npoints / size;

    if (size < 2) {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    // Inicjalizacja generatora liczb pseudolosowych
    srand(rank * time(NULL));

    start = MPI_Wtime(); // Rozpoczęcie pomiaru czasu

    for (long j = 0; j < num; j++) {
        // Generowanie dwóch liczb losowych w zakresie [0, 1]
        xcoordinate = (double)rand() / RAND_MAX;
        ycoordinate = (double)rand() / RAND_MAX;

        // Sprawdzenie, czy punkt znajduje się wewnątrz koła (o promieniu 0.5)
        if (xcoordinate * xcoordinate + ycoordinate * ycoordinate <= 1) {
            circle_count++;
        }
    }

    end = MPI_Wtime(); // Zakończenie pomiaru czasu
    cpu_time_used = end - start;

    // Zbieranie wyników z każdego procesora
    long total_circle_count;
    MPI_Reduce(&circle_count, &total_circle_count, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // Bariera przed obliczeniem przybliżonej wartości liczby π
    MPI_Barrier(MPI_COMM_WORLD);

    // Obliczenie przybliżonej wartości liczby π tylko na procesie o rankingu 0
    if (rank == 0) {
        double pi = 4.0 * total_circle_count / npoints;
        if (pi >= 0.4 && pi <= 4.0) {
            printf("Liczba PI obliczona metoda Monte Carlo: %f\n", pi);
        } else {
            printf("Błąd: Wyliczona wartość liczby PI (%f) nie mieści się w zakresie [0.4, 4.0]\n", pi);
        }
        printf("Liczba punktów: %ld\n", npoints);
        printf("Czas wykonania: %f sekund\n", cpu_time_used);
        printf("Liczba użytych procesorów: %d\n", size);
    }

    MPI_Finalize();
    return 0;
}
