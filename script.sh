#!/bin/bash

# Inicializar variables para almacenar sumas y conteos
declare -a sums=(0 0 0 0 0)
total_time=0
count=100

for i in $(seq 1 $count); do
    # Capturar el tiempo de inicio
    start_time=$(date +%s.%N)

    # Reemplazar por instancia
    output=$(./main 500 100 100 1.5 100 2>&1)
    
    # Manejar errores en la ejecución
    if [ $? -ne 0 ]; then
        echo "Error en la ejecución: $output"
        continue
    fi

    # Leer los valores de salida (suponiendo que son números en líneas separadas)
    readarray -t values <<< "$output"

    # Sumar los valores a las respectivas posiciones
    for j in ${!values[@]}; do
        sums[$j]=$(echo "${sums[$j]} + ${values[$j]}" | bc)
    done

    # Capturar el tiempo de fin y calcular el tiempo transcurrido
    end_time=$(date +%s.%N)
    elapsed_time=$(echo "$end_time - $start_time" | bc)
    total_time=$(echo "$total_time + $elapsed_time" | bc)
done

# Calcular promedios
echo "Promedios de los valores:"
for j in ${!sums[@]}; do
    average=$(echo "${sums[$j]} / $count" | bc -l)
    printf "Valor %d: %.2f\n" $((j + 1)) "$average"
done

# Calcular y mostrar el tiempo promedio de ejecución
average_time=$(echo "$total_time / $count" | bc -l)
printf "Tiempo promedio de ejecución: %.2f segundos\n" "$average_time"