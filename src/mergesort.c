#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "mergesort.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_attr_t attr;

volatile unsigned short int avaliable_threads = MAX_THREADS;

void* merge(int* arr, int init, int mid, int end) {
    int arr1_elements = mid - init + 1;
    int arr2_elements = end - mid;
    int* arr_temp = (int*) calloc(arr1_elements+arr2_elements, sizeof(int));
    if (!arr_temp) {
        printf("Erro ao tentar alocar memória na função merge!\n");
        exit(-1);
    }
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

void* __mergesort_threaded__(void* arg) {
    thread_data* data = (thread_data*) arg;
    int init = data->init;
    int end = data->end;
    int* arr = data->arr;
    free(data);
    if (init < end) {
        int mid = (end + init) / 2;
        pthread_mutex_lock(&lock);
        if ((end - init) > THRESHOLD && avaliable_threads > 1) {
            avaliable_threads -= 2;
            pthread_mutex_unlock(&lock);
            thread_data* left_data = malloc(sizeof(thread_data));
            thread_data* right_data = malloc(sizeof(thread_data));
            if (!left_data || !right_data) {
                printf("Erro ao tentar alocar memória na função __mergesort_threaded__!\n");
                exit(-1);
            }
            *left_data = (thread_data){arr, init, mid};
            *right_data = (thread_data){arr, mid + 1, end};
            pthread_t left_thread, right_thread;
            if (pthread_create(&left_thread, &attr, __mergesort_threaded__, left_data) != 0) {
                perror("Erro ao criar thread para subarray esquerdo");
                free(left_data);
                free(right_data);
                exit(-1);
            }            
            if (pthread_create(&right_thread, &attr, __mergesort_threaded__, right_data) != 0) {
                perror("Erro ao criar thread para subarray direito");
                free(left_data);
                free(right_data);
                exit(-1);
            }
            pthread_join(left_thread, NULL);
            pthread_join(right_thread, NULL);
        } else {
            pthread_mutex_unlock(&lock);
            mergesort(arr, init, mid);
            mergesort(arr, mid+1, end);
        }
        merge(arr, init, mid, end);
    }
    printf("Finalizei uma execução!\n");
    return NULL;
}

void* __mergesort_with_thread__(int* arr, int init, int end) {
    thread_data* data = malloc(sizeof(thread_data));
    if (!data) {
        printf("Erro ao tentar alocar memória na função __mergesort_with_thread__!\n");
        exit(-1);
    }
    *data = (thread_data){arr, init, end};
    __mergesort_threaded__(data);
    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    unsigned short int print_arrays = 0;
    switch (argc) {
    case 1:
        print_arrays = 0;
        break;
    case 2:
        if (atoi(argv[1]) > 1) {
            print_arrays = atoi(argv[1]);
        } else {
            print_arrays = 1;
        }
        break;
    default:
        printf("Parâmetros incorretos!\n");
        return -1;
        break;
    }
    srand(time(NULL));
    int* array1 = (int*) malloc(sizeof(int) * LIMIT);
    int* array2 = (int*) malloc(sizeof(int) * LIMIT);
    if (!array1 || !array2) {
        printf("Erro ao alocar memória!\n");
        return -1;
    }
    for (int i = 0; i < LIMIT; i++) {
        array1[i] = LIMIT - i;
        // array1[i] = (rand() % LIMIT) + 1;
        array2[i] = array1[i];
    }
    if (print_arrays == 2) {
        for (int i = 0; i < LIMIT; i++) {
            printf("%d ", array2[i]);
        }
        printf("\n");
    }
    clock_t start_thread = clock();
    __mergesort_with_thread__(array2, 0, LIMIT - 1);
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
    free(array1);
    free(array2);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    pthread_attr_destroy(&attr);
    return 0;
}
