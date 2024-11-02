#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "mergesort.h"

pthread_mutex_t lock;
pthread_cond_t cond;

int avaliable_threads = MAX_THREADS;

int verify_sorted_array(int* arr) {
    int size = LIMIT;
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i+1] && i != size-1) {
            return 1;
        }
    }
    return 0;
}

void* merge(int* arr, int init, int mid, int end) {
    int arr1_elements = mid - init + 1;
    int arr2_elements = end - mid;
    int* arr_temp = (int*) calloc((arr1_elements+arr2_elements), sizeof(int));
    if (!arr_temp) { exit(-1); }
    int iterator_to_init = 0, iterator_to_mid = 0, iterator_global = 0;
    while (iterator_to_init < arr1_elements && iterator_to_mid < arr2_elements) {
        if (arr[init+iterator_to_init] > arr[mid+1+iterator_to_mid]) {
            arr_temp[iterator_global] = arr[mid+1+iterator_to_mid];
            iterator_to_mid++;
        } else {
            arr_temp[iterator_global] = arr[init+iterator_to_init];
            iterator_to_init++;
        }
        iterator_global++;
    }
    while (iterator_to_init < arr1_elements) {
        arr_temp[iterator_global] =  arr[init+iterator_to_init];
        iterator_to_init++;
        iterator_global++;
    }
    while (iterator_to_mid < arr2_elements) {
        arr_temp[iterator_global] = arr[mid+1+iterator_to_mid];
        iterator_to_mid++;
        iterator_global++;
    }
    for (int i = init; i <= end; i++) {
        arr[i] = arr_temp[i - init];
    }
    free(arr_temp);
    return NULL;
}

void* mergesort(int* arr, int init, int end) {
    if (init < end) {
        int mid = (end + init) / 2;
        mergesort(arr, init, mid);
        mergesort(arr, mid+1, end);
        merge(arr, init, mid, end);
    }
    return NULL;
}


void isort(int* arr, int size) {
    for (int i = 1; i < size; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

int compare(const void* a, const void* b) {
    return (*(int*) a - *(int*)b);
}

void* sort_option(void* arg) {
    thread_data* data = (thread_data*) arg;
    if ((data->end - data->init) > 15) {
        qsort(data->arr+data->init, (data->end-data->init+1), sizeof(int), compare);
    } else {
        isort(data->arr, (data->end-data->init+1));
    }
    return NULL;
}

void* mergesort_threaded(int* arr, int init, int end, int nthreads) {
    pthread_t thread[nthreads];
    thread_data* datas[nthreads];
    for (int i = 0; i < nthreads; i++) {
        thread_data* data = (thread_data*) malloc(sizeof(thread_data));
        if (!data) { exit(-1); }
        datas[i] = data;
        if (i == nthreads - 1) {
            *data = (thread_data){arr, ((end + init)/nthreads)*i, end};
        } else {
            *data = (thread_data){arr, ((end + init)/nthreads)*i, (((end + init)/nthreads)*(i+1))-1};
        }
        if (pthread_create(&thread[i], NULL, sort_option, data) != 0) {
            for (int j = i; j <= 0; j--) {
                free(datas[j]);
            }
            exit(-1);
        }
    }
    for (int i = 0; i < nthreads; i++) {
        pthread_join(thread[i], NULL);
        free(datas[i]);
    }
    int step = (end + init) / nthreads;
    for (int size = step; size < (end + init); size *= 2) {
        for (int start = 0; start < (end + init); start += 2 * size) {
            int arr_mid = start + size - 1;
            int arr_end = (start + 2 * size - 1 < (end + init)) ? start + 2 * size - 1 : (end + init) - 1;
            if (arr_mid < arr_end) {
                merge(arr, start, arr_mid, arr_end);
            }
        }
    }
    return NULL;
}

void* __mergesort_threaded__(void* arg) {
    thread_data* data = (thread_data*) arg;
    int init = data->init;
    int end = data->end;
    if (init >= end) {
        return NULL;
    }
    int mid = (end + init) / 2;
    int* arr = data->arr;
    thread_data* left_data = malloc(sizeof(thread_data));
    thread_data* right_data = malloc(sizeof(thread_data));
    if (!left_data || !right_data) {
        printf("Erro ao tentar alocar memória na função __mergesort_threaded__!\n");
        exit(-1);
    }
    *left_data = (thread_data){arr, init, mid};
    *right_data = (thread_data){arr, mid + 1, end};
    pthread_mutex_lock(&lock);
    pthread_t right_thread;
    if ((end - init) > THRESHOLD && avaliable_threads > 0) {
        avaliable_threads -= 1;
        pthread_mutex_unlock(&lock);
        pthread_create(&right_thread, NULL, __mergesort_threaded__, right_data);
        __mergesort_threaded__(left_data);
        pthread_join(right_thread, NULL);
    } else {
        pthread_mutex_unlock(&lock);
        __mergesort_threaded__(left_data);
        __mergesort_threaded__(right_data);
    }
    merge(arr, init, mid, end);
    free(data);
    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);
    unsigned short int print_arrays;
    print_arrays = (argc == 1) ? 0 : (atoi(argv[1]) > 1 ? atoi(argv[1]) : 1);
    if (argc > 2) {
        printf("Parâmetros incorretos!\n");
        return -1;
    }
    srand(time(NULL));
    int* array1 = (int*) malloc(sizeof(int) * LIMIT);
    int* array2 = (int*) malloc(sizeof(int) * LIMIT);
    if (!array1 || !array2) { exit(-1); }
    for (int i = 0; i < LIMIT; i++) {
        // array1[i] = LIMIT - i;
        // array1[i] = i+1;
        array1[i] = (rand() % LIMIT) + 1;
        array2[i] = array1[i];
    }
    if (print_arrays == 2) {
        for (int i = 0; i < LIMIT; i++) {
            printf("%d ", array2[i]);
        }
        printf("\n");
    }
    //thread_data* data = (thread_data*) malloc(sizeof(thread_data));
    //*data = (thread_data){array2, 0, LIMIT-1};
    //if (!data) { exit(-1); }
    clock_t start_thread = clock();
    mergesort_threaded(array2, 0, LIMIT, MAX_THREADS);
    clock_t end_thread = clock();
        if (print_arrays == 2) {
        for (int i = 0; i < LIMIT; i++) {
            printf("%d ", array2[i]);
        }
        printf("\n");
    }
    double time_thread = (double)(end_thread - start_thread) / CLOCKS_PER_SEC;
    if (print_arrays == 3) {
        for (int i = 0; i < LIMIT; i++) {
            printf("%d ", array1[i]);
        }
        printf("\n");
    }
    clock_t start_seq = clock();
    mergesort(array1, 0, LIMIT - 1);
    clock_t end_seq = clock();
    if (print_arrays == 3) {
        for (int i = 0; i < LIMIT; i++) {
            printf("%d ", array1[i]);
        }
        printf("\n");
    }
    double time_seq = (double)(end_seq - start_seq) / CLOCKS_PER_SEC;
    printf("%f,%f\n", time_seq, time_thread);
    printf("Verify sorted array return: %d\n", verify_sorted_array(array2));
    free(array1);
    free(array2);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    return 0;
}
