#include <stdio.h>
#include <stdlib.h>
#include <time.h>



int main() {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <number_of_points>\n", argv[0]);
        return 1;
    }
    long npoints = atol(argv[1]);
    long circle_count = 0;
    double xcoordinate, ycoordinate;
    clock_t start, end;
    double cpu_time_used;
    int size = 1;
    // Inicjalizacja generatora liczb pseudolosowych
    srand(time(NULL));

    start = clock(); // Rozpoczęcie pomiaru czasu

    for (long j = 0; j < npoints; j++) {
        // Generowanie dwóch liczb losowych w zakresie [0, 1]
        xcoordinate = (double)rand() / RAND_MAX;
        ycoordinate = (double)rand() / RAND_MAX;

        // Sprawdzenie, czy punkt znajduje się wewnątrz koła (o promieniu 0.5)
        if (xcoordinate * xcoordinate + ycoordinate * ycoordinate <= 1) {
            circle_count++;
        }
    }

    end = clock(); // Zakończenie pomiaru czasu
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    // Obliczenie przybliżonej wartości liczby pi
    double pi = 4.0 * circle_count / npoints;

    // Sprawdzenie, czy wyliczona wartość liczby π znajduje się w zakresie [0.4]
    if (pi >= 0.4 && pi <= 4.0) {
        printf("Liczba PI obliczona metoda Monte Carlo: %f\n", pi);
    } else {
        printf("Błąd: Wyliczona wartość liczby PI (%f) nie mieści się w zakresie [0.4, 4.0]\n", pi);
    }

    printf("Liczba punktów: %ld\n", npoints); // Dodatkowy wiersz wydruku
    printf("Czas wykonania: %f sekund\n", cpu_time_used);
    printf("Liczba użytych procesorów: %d\n", size);

    return 0;
}
