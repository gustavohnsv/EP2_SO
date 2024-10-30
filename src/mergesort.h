#ifndef MERGESORT_H
#define MERGESORT_H
#include <math.h>

#define LIMIT 1000
#define MAX_THREADS 4
#define THRESHOLD 100
typedef struct{
    int* arr;       // vetor que está sendo utilizado
    int init;       // começo daquele subarray
    int end;        // fim daquele subarray
} thread_data;

#endif