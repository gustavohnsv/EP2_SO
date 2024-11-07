#ifndef MERGESORT_H
#define MERGESORT_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>    
#include <sys/stat.h>
#include <string.h>

#define REG_SIZE 100
#define REG_DATA_BUFFER 96

typedef struct {
    __uint32_t key; // chave do registro
    char data[96];  // campo para dados do registro
} reg;

typedef struct {
    reg* arr;       // vetor que está sendo utilizado
    int init;       // começo daquele subarray
    int end;        // fim daquele subarray
} thread_data;

// funções de suporte
int     verify_sorted_array(reg* arr, int size);
void    print_error_and_exit(char* msg);
void*   print_array(reg* arr, int size);
void    verify_range(reg* arr, int start, int end, const char* msg);

// funções de manipulação de memória e de arquivos
int     open_file(char* filename, int flags, mode_t mode);
void*   map_file(int fd, size_t size);
void    write_file(char* filename, void* data, size_t size);

// funções para ordenação do conjunto de dados
int     compare(const void* a, const void* b);
void*   merge(reg* arr, int init, int mid, int end);
void*   mergesort(reg* arr, int init, int end);
void    isort(reg* arr, int init, int end);
void*   sort_option(void* arg);
void*   mergesort_threaded(reg* arr, int init, int end, int nthreads);

#endif