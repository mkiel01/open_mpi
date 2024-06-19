#!/bin/bash

echo "Starting the script to compile and run MPI programs..."

# Updated program array with new suffix
all_programs=(calc_p_1_w.c calc_p_10_w.c calc_p_60_w.c calc_s_1_w.c calc_s_10_w.c calc_s_60_w.c)

# Initialize or clear the CSV file
echo "PI, Points, Execution Time, Processors" > results_w.csv

# Loop through each program
for program in "${all_programs[@]}"
do
    # Extract the base name without the .c extension
    base_name=$(echo "$program" | cut -f 1 -d '.')

    # Compile the program using mpicc
    mpicc -o "${base_name}_c" "$program"

    # Determine the base number of points based on the file name
    if [[ $program == *"1_w.c" ]]; then
        base_npoints=61500000 # For programs ending with 1
    elif [[ $program == *"10_w.c" ]]; then
        base_npoints=615000000 # For programs ending with 10
    elif [[ $program == *"60_w.c" ]]; then
        base_npoints=4270000000 # For programs ending with 60
    fi

    # Determine the correct execution based on the program type
    if [[ $program == calc_s_* ]]; then
        # Sequential programs, executed with -np 1
        output=$(mpiexec -machinefile ./1_node_with_8_process -np 1 "./${base_name}_c" $base_npoints)
                # Extract data from the output and append to the CSV file
        pi=$(echo "$output" | grep -o "Liczba PI obliczona metoda Monte Carlo: [0-9.]*" | awk '{print $NF}')
        points=$(echo "$output" | grep -o "Liczba punktów: [0-9]*" | awk '{print $NF}')
        time=$(echo "$output" | grep -o "Czas wykonania: [0-9.]*" | awk '{print $NF}')
        processors=$(echo "$output" | grep -o "Liczba użytych procesorów: [0-9]*" | awk '{print $NF}')
        if [[ -n "$pi" && -n "$points" && -n "$time" && -n "$processors" ]]; then
            echo "$pi, $points, $time, $processors" >> results_w.csv
        fi
    else
        # Parallel programs, executed with varying numbers of processors from 2 to 8
        for np in {2..8}
        do
            # Weak scaling: scale npoints with the number of processors
            npoints=$(($base_npoints * $np))
            output=$(mpiexec -machinefile ./1_node_with_8_process -np "$np" "./${base_name}_c" $npoints)
            # Extract data from the output and append to the CSV file
            pi=$(echo "$output" | grep -o "Liczba PI obliczona metoda Monte Carlo: [0-9.]*" | awk '{print $NF}')
            points=$(echo "$output" | grep -o "Liczba punktów: [0-9]*" | awk '{print $NF}')
            time=$(echo "$output" | grep -o "Czas wykonania: [0-9.]*" | awk '{print $NF}')
            processors=$(echo "$output" | grep -o "Liczba użytych procesorów: [0-9]*" | awk '{print $NF}')

            if [[ -n "$pi" && -n "$points" && -n "$time" && -n "$processors" ]]; then
                echo "$pi, $points, $time, $processors" >> results_w.csv
            fi
        done
    fi
done

echo "Script has completed execution."
