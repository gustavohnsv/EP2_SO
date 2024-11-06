#!/bin/bash

PROGRAM="mergesort.exe"
OUTPUT_FILE="saida_programa.csv"

if [ -f "$OUTPUT_FILE" ]; then
    rm "$OUTPUT_FILE"
fi

total_time=0
count=0

for i in {1..50}; do
    nice -19 ./${PROGRAM} 123_registros_aleatorios.dat a.txt 8 >> "$OUTPUT_FILE"
done

echo "Execuções concluídas. Saída do programa registrada em $OUTPUT_FILE."