#!/bin/bash
files=("matrix_mult" "matrix_mult_v1" "matrix_mult_v2") 
sizes=(1024 2048 4096)
tests=3
csv="benchmark"

# Compilazione file C
for file in "${files[@]}"; do
    echo `g++ -o ${file} ${file}.cpp -fopenmp`;
done

for file in "${files[@]}"; do   
    # Creazione Headers nel file di output
    filename="${csv}_${file}_without_flag_16.csv"
    echo "N, t1, t2, t3" >> ${filename}

    for size in "${sizes[@]}"; do
        line="${size}, "
        for ((i=0;i<${tests};i++)); do
            last_time=`./${file} ${size} 16`
            line="${line}${last_time}, "  
            echo "Algoritmo {$file} size: ${size} test: ${i} time: ${last_time}"
        done
        end=$(( ${#line} - 2 ))
        echo "${line:0:${end}}" >> ${filename}
    done
done
