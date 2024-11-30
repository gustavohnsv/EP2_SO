#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

terminal_width=$(tput cols)
text_length=${#TEXT}
padding=$(( (terminal_width - text_length) / 2 ))

PROGRAM="psort14713982.exe"
PROGRAM_EXIT="registros_ordenados.out"
TEXTS=("Execução para um vetor de mil elementos" "Execução para um vetor de 10 mil elementos" 
        "Execução para um vetor de 100 mil elementos" "Execução para um vetor de 1 milhão de elementos")
INPUT_FILES=("1K_registros.dat" "10K_registros.dat" "100K_registros.dat" 
                "1M_registros.dat")
OUTPUT_FILES=("saida_1K.csv" "saida_10K.csv" "saida_100K.csv" "saida_1M.csv")
INPUT_FILES_DIR=data

get_terminal_width() {
    tput cols
}

show_progress() {
    local progress=$1
    local total=$2
    local terminal_width=$(get_terminal_width)
    local percentage=$((progress * 100 / total))

    local bar_width=$((terminal_width - 10)) # Reservar espaço para " 100%"
    local filled_width=$((percentage * bar_width / 100))
    local empty_width=$((bar_width - filled_width))

    printf "\r[%s%s] %3d%%" "$(printf '#%.0s' $(seq 1 $filled_width))" "$(printf '.%.0s' $(seq 1 $empty_width))" "$percentage"
}

for i in {1..4}; do
    if [ -f "${OUTPUT_FILES[i-1]}" ]; then
        rm "${OUTPUT_FILES[i-1]}"
    fi
done

current_execution=0

umount /mnt/ramdisk 2>/dev/null
rm -rf /mnt/ramdisk

mkdir -p /mnt/ramdisk
mount -t tmpfs -o size=4G tmpfs /mnt/ramdisk

for k in {1..4}; do
    clear
    echo "num_threads,exec,real_time,usr_time,sys_time" >> "${OUTPUT_FILES[k-1]}"
    echo ""
    printf "${GREEN}%-20s${NC}\n" "[EXEC] - ${TEXTS[k-1]}"
    TOTAL_EXECUTIONS=$((12 * 50))
    current_execution=0
    for i in {1..12}; do
        for j in {1..50}; do
            cp ${INPUT_FILES_DIR}/${INPUT_FILES[k-1]} /mnt/ramdisk/
            { /usr/bin/time -f "%e,%U,%S" taskset -c 0-$(nproc --all) nice -n -19 ./${PROGRAM} /mnt/ramdisk/${INPUT_FILES[k-1]} ${PROGRAM_EXIT} ${i} > /dev/null; } 2>&1 | \
            awk -v threads="$i" -v exec="$j" -F',' '{ printf "%d,%d,%s,%s,%s\n", threads, exec, $1, $2, $3 }' >> "${OUTPUT_FILES[k-1]}"
            if [ $? -ne 0 ]; then
                echo "Erro na execução com threads=$i, exec=$j" >> "${OUTPUT_FILES[k-1]}"
            fi
            current_execution=$((current_execution + 1))
            show_progress "$current_execution" "$TOTAL_EXECUTIONS"
        done
    done
    current_execution=0
    echo ""
done

umount /mnt/ramdisk
rm -rf /mnt/ramdisk

clear
rm "$PROGRAM_EXIT"

printf "${GREEN}%-20s${NC}\n" "[EXEC] - Todas as execuções foram concluídas!"