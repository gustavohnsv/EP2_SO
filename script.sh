#!/bin/bash

PROGRAM="psort14713982.exe"
PROGRAM_EXIT="registros_ordenados.out"
OUTPUT_FILE="saida_programa.csv"

if [ -f "$OUTPUT_FILE" ]; then
    rm "$OUTPUT_FILE"
fi

echo "num_threads,exec,real_time,usr_time,sys_time" >> "$OUTPUT_FILE"


for i in {1..12}; do
    for j in {1..50}; do
        { /usr/bin/time -f "%e,%U,%S" nice -19 ./${PROGRAM} 1K_registros.dat ${PROGRAM_EXIT} ${i} > /dev/null; } 2>&1 | \
        awk -v threads="$i" -v exec="$j" -F',' '{ printf "%d,%d,%s,%s,%s\n", threads, exec, $1, $2, $3 }' >> "$OUTPUT_FILE"
        if [ $? -ne 0 ]; then
            echo "Erro na execução com threads=$i, exec=$j" >> "$OUTPUT_FILE"
        fi
    done
done

rm "$PROGRAM_EXIT"

echo "Execuções concluídas. Saída do programa registrada em $OUTPUT_FILE."