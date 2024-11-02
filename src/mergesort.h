#ifndef MERGESORT_H
#define MERGESORT_H

#define LIMIT 123456
#define MAX_THREADS 4
#define THRESHOLD LIMIT/MAX_THREADS
typedef struct{
    int* arr;       // vetor que está sendo utilizado
    int init;       // começo daquele subarray
    int end;        // fim daquele subarray
} thread_data;

void*   merge(int* arr, int init, int mid, int end);
void*   mergesort(int* arr, int init, int end);
void*   sort_option(void* arg);
void    isort(int* arr, int size);
int     compare(const void* a, const void* b);
void*   mergesort_threaded(int* arr, int init, int end, int nthreads);
void*   __mergesort_threaded__(void* arg);

#endif