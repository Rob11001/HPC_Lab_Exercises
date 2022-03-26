#!/bin/bash
files=("dot_product" "dot_product_avx")
sizes=(67108864 134217728 268435456 536870912)
tests=3
csv="benchmark"

# Compilazione file C
for file in "${files[@]}"; do
    echo `g++ -o ${file}.out ${file}.cpp -mfma -mavx -O0`;
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
