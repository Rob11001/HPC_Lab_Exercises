#!/bin/bash
files=("complex_number_aos" "complex_number_soa")
sizes=(8000000 16000000 32000000 64000000 128000000)
tests=3
csv="benchmark"

# Compilazione file C
for file in "${files[@]}"; do
    echo `g++ -o ${file}.out ${file}.cpp -mavx`;
done

for file in "${files[@]}"; do   
    # Creazione Headers nel file di output
    filename="${csv}_${file}.csv"
    echo "N, t1, t2, t3" >> ${filename}

    for size in "${sizes[@]}"; do
        line="${size}, "
        for ((i=0;i<${tests};i++)); do
            last_time=`./${file}.out ${size}` 
            line="${line}${last_time}, "  
            echo "Algoritmo {$file} size: ${size} test: ${i} time: ${last_time}"
        done
        end=$(( ${#line} - 2 ))
        echo "${line:0:${end}}" >> ${filename}
    done
done
