#!/bin/bash

PROGRAM="mergesort.exe"
OUTPUT_FILE="saida_programa.txt"

if [ -f "$OUTPUT_FILE" ]; then
    rm "$OUTPUT_FILE"
fi

total_time=0
count=0

for i in {1..50}; do
    start_time=$(date +%s.%N)
    ./$PROGRAM 0 >> "$OUTPUT_FILE"
    end_time=$(date +%s.%N)

    execution_time=$(echo "$end_time - $start_time" | bc)
    total_time=$(echo "$total_time + $execution_time" | bc)
    count=$((count + 1))
done

echo "Execuções concluídas. Saída do programa registrada em $OUTPUT_FILE."