

echo "Starting the script to compile and run MPI programs..."

all_programs=(calc_p_1.c calc_p_10.c calc_p_60.c calc_s_1.c calc_s_10.c calc_s_60.c)

# Initialize or clear the CSV file
echo "PI, Points, Execution Time, Processors" > results.csv

# Loop through each program
for program in "${all_programs[@]}"
do
    # Extract the base name without the .c extension
    base_name=$(echo "$program" | cut -f 1 -d '.')

    # Compile the program using mpicc
    mpicc -o "${base_name}_c" "$program"

    if [[ $program == calc_s_* ]]; then
        # Run the sequential program with -np 1
        output=$(mpiexec -machinefile ./1_node_with_8_process -np 1 "./${base_name}_c")
        # Extract data from the output and append to the CSV file
        pi=$(echo "$output" | grep -o "Liczba PI obliczona metoda Monte Carlo: [0-9.]*" | awk '{print $NF}')
        points=$(echo "$output" | grep -o "Liczba punktów: [0-9]*" | awk '{print $NF}')
        time=$(echo "$output" | grep -o "Czas wykonania: [0-9.]*" | awk '{print $NF}')
        processors=$(echo "$output" | grep -o "Liczba użytych procesorów: [0-9]*" | awk '{print $NF}')
        
        if [[ -n "$pi" && -n "$points" && -n "$time" && -n "$processors" ]]; then
            echo "$pi, $points, $time, $processors" >> results.csv
        fi
    else
        # Run the parallel program with varying numbers of processes from 2 to 8
        for np in {2..8}
        do
            output=$(mpiexec -machinefile ./1_node_with_8_process -np "$np" "./${base_name}_c")
            # Extract data from the output and append to the CSV file
            pi=$(echo "$output" | grep -o "Liczba PI obliczona metoda Monte Carlo: [0-9.]*" | awk '{print $NF}')
            points=$(echo "$output" | grep -o "Liczba punktów: [0-9]*" | awk '{print $NF}')
            time=$(echo "$output" | grep -o "Czas wykonania: [0-9.]*" | awk '{print $NF}')
            processors=$(echo "$output" | grep -o "Liczba użytych procesorów: [0-9]*" | awk '{print $NF}')

            if [[ -n "$pi" && -n "$points" && -n "$time" && -n "$processors" ]]; then
                echo "$pi, $points, $time, $processors" >> results.csv
            fi
        done
    fi
done
