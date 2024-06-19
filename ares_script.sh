#!/bin/bash -l
#SBATCH --nodes 1
#SBATCH --ntasks 12
#SBATCH --time=01:00:00
#SBATCH --partition=plgrid
#SBATCH --account=plgmpr24-cpu

module add .plgrid plgrid/tools/openmpi

echo "Starting the script to compile and run MPI programs..."

all_programs=(calc_p_1.c calc_p_10.c calc_p_60.c calc_s_1.c calc_s_10.c calc_s_60.c)


echo "PI, Points, Execution Time, Processors" > ares_results.csv

for program in "${all_programs[@]}"
do

    base_name=$(echo "$program" | cut -f 1 -d '.')

    mpicc -o "${base_name}_c" "$program"

    if [[ $program == calc_s_* ]]; then
   
        output=$(mpiexec -np 1 "./${base_name}_c")
    
        pi=$(echo "$output" | grep -o "Liczba PI obliczona metoda Monte Carlo: [0-9.]*" | awk '{print $NF}')
        points=$(echo "$output" | grep -o "Liczba punktów: [0-9]*" | awk '{print $NF}')
        time=$(echo "$output" | grep -o "Czas wykonania: [0-9.]*" | awk '{print $NF}')
        processors=$(echo "$output" | grep -o "Liczba użytych procesorów: [0-9]*" | awk '{print $NF}')
        
        if [[ -n "$pi" && -n "$points" && -n "$time" && -n "$processors" ]]; then
            echo "$pi, $points, $time, $processors" >> ares_results.csv
        fi
    else
  
        for np in {2..8}
        do
            output=$(mpiexec -np "$np" "./${base_name}_c")
        
            pi=$(echo "$output" | grep -o "Liczba PI obliczona metoda Monte Carlo: [0-9.]*" | awk '{print $NF}')
            points=$(echo "$output" | grep -o "Liczba punktów: [0-9]*" | awk '{print $NF}')
            time=$(echo "$output" | grep -o "Czas wykonania: [0-9.]*" | awk '{print $NF}')
            processors=$(echo "$output" | grep -o "Liczba użytych procesorów: [0-9]*" | awk '{print $NF}')

            if [[ -n "$pi" && -n "$points" && -n "$time" && -n "$processors" ]]; then
                echo "$pi, $points, $time, $processors" >> ares_results.csv
            fi
        done
    fi
done
