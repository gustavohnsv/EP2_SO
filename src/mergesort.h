#ifndef MERGESORT_H
#define MERGESORT_H

#define LIMIT 123456
#define MAX_THREADS 4
#define THRESHOLD LIMIT/MAX_THREADS

#define REG_SIZE 100
#define REG_DATA_BUFFER 96

typedef struct {
    __uint32_t key;
    char data[96];
} reg;

typedef struct {
    reg* arr;       // vetor que está sendo utilizado
    int init;       // começo daquele subarray
    int end;        // fim daquele subarray
} thread_data;

void* merge(reg* arr, int init, int mid, int end);
void* mergesort(reg* arr, int init, int end);
void isort(reg* arr, int init, int end);
int compare(const void* a, const void* b);
void* sort_option(void* arg);
void* mergesort_threaded(reg* arr, int init, int end, int nthreads);

void verify_range(reg* arr, int start, int end, const char* message);
int verify_sorted_array(reg* arr, int size);
void* print_array(reg* arr, int size);

#endif