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
    char data[REG_DATA_BUFFER];     // registros reais
} reg_data;

typedef struct {
    __uint32_t key;                 // chave do registro
    reg_data* data;                 // ponteiro para os dados reais
} reg;
typedef struct {
    __uint32_t key;                 // chave do registro
    char data[REG_DATA_BUFFER];     // campo para dados do registro
} reg_w_data;

typedef struct {
    reg* arr;       // vetor que está sendo utilizado
    int init;       // começo daquele subarray
    int end;        // fim daquele subarray
} thread_data;

// funções de suporte
int     verify_sorted_array(reg* arr, int size);
void    print_error_and_exit(char* msg);
void    print_array(reg* arr, int size);
void    verify_range(reg* arr, int start, int end, const char* msg);

// funções de manipulação de memória e de arquivos
int     open_file(char* filename, int flags, mode_t mode);
void*   map_file(int fd, size_t size, int flag);
void    write_file(char* filename, void* data, size_t size);
void    data_to_pointer(reg_w_data* arr1, reg* arr2, int size);
void    pointer_to_data(reg* arr1, reg_w_data* arr2, int size);

// funções para ordenação do conjunto de dados
int     compare(const void* a, const void* b);
void    merge(reg* arr, reg* arr_temp, int init, int mid, int end);
void    isort(reg* arr, int init, int end);
void*   sort_option(void* arg);
void    threaded_sort(reg* arr, int init, int end, int nthreads);