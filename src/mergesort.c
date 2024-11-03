#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <math.h>

#include "mergesort.h"

void* print_array(reg* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("[%03d°] - Chave: %u\n", i+1, arr[i].key);
        /* dados
        for (int k = 0; k < REG_DATA_BUFFER; k++) {
            printf("%02x", (unsigned char) regs[i].data[k]);
        }
        printf("\n");
        */
    }
    return NULL;
}

int verify_sorted_array(reg* arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (arr[i].key > arr[i+1].key) {
            return 1;
        }
    }
    return 0;
}

void* merge(reg* arr, int init, int mid, int end) {
    int arr1_elements = mid - init + 1;
    int arr2_elements = end - mid;
    reg* arr_temp = (reg*) calloc((arr1_elements+arr2_elements), sizeof(reg));
    if (!arr_temp) { exit(-1); }
    int iterator_to_init = 0, iterator_to_mid = 0, iterator_global = 0;
    while (iterator_to_init < arr1_elements && iterator_to_mid < arr2_elements) {
        if (arr[init+iterator_to_init].key > arr[mid+1+iterator_to_mid].key) {
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
    print_array(arr_temp, (end-init));
    free(arr_temp);
    return NULL;
}

void* mergesort(reg* arr, int init, int end) {
    if (init < end) {
        int mid = (end + init) / 2;
        mergesort(arr, init, mid);
        mergesort(arr, mid+1, end);
        merge(arr, init, mid, end);
    }
    return NULL;
}

void isort(reg* arr, int init, int end) {
    for (int i = init; i < end+1; i++) {
        reg aux = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].key > aux.key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = aux;
    }
}

int compare(const void* a, const void* b) {
    return ((reg*) a)->key - ((reg*) b)->key;
}

void* sort_option(void* arg) {
    thread_data* data = (thread_data*) arg;
    if ((data->end - data->init) > 15) {
        qsort(data->arr+data->init, (data->end-data->init+1), sizeof(reg), compare);
    } else {
        isort(data->arr, data->init, data->end);
    }
    return NULL;
}

void* mergesort_threaded(reg* arr, int init, int end, int nthreads) {
    pthread_t thread[nthreads];
    thread_data* datas[nthreads];
    if (nthreads > (end - init + 1)) {
        nthreads = (end - init + 1);
    }
    int chunk_size = (end - init + 1) / nthreads;
    if (chunk_size < 1) chunk_size = 1;    
    for (int i = 0; i < nthreads; i++) {
        thread_data* data = (thread_data*) malloc(sizeof(thread_data));
        if (!data) { exit(-1); }
        datas[i] = data;
        data->arr = arr;
        data->init = init + (i * chunk_size);
        data->end = (i == nthreads - 1) ? end : (init + (i + 1) * chunk_size - 1);        
        // printf("init: %d, end: %d\n", data->init, data->end);
        if (pthread_create(&thread[i], NULL, sort_option, data) != 0) {
            for (int j = 0; j < i; j--) {
                free(datas[j]);
            }
            exit(-1);
        }
    }
    for (int i = 0; i < nthreads; i++) {
        pthread_join(thread[i], NULL);
        free(datas[i]);
    }
    for (int size = chunk_size; size <= (end - init); size = size * 2) {
        for (int left = init; left <= end - size; left += size * 2) {
            int mid = left + size - 1;
            int right = (left + (2 * size) - 1 <= end) ? left + (2 * size) - 1 : end;
            merge(arr, left, mid, right);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Parâmetros incorretos!\n");
        exit(-1);
    }
    char* read_filename = argv[1];
    //char* write_filename = argv[2];
    int threads = atoi(argv[3]);
    FILE* fr = fopen(read_filename, "rb");
    if (!fr) {
        printf("Erro ao ler o arquivo!\n");
    }
    fseek(fr, 0, SEEK_END);
    long fileSize = ftell(fr);
    fseek(fr, 0, SEEK_SET);
    int num_regs = fileSize / REG_SIZE;
    reg* regs = (reg*) malloc(sizeof(reg)*num_regs);
    if (!regs) {
        fclose(fr);
        exit(-1);
    }
    for (int i = 0; i < num_regs; i++) {
        if (fread(&regs[i], REG_SIZE, 1, fr) == 1) {
            continue;
        } else {
            exit(-1);
        }
    }
    mergesort_threaded(regs, 0, num_regs-1, threads);
    // print_array(regs, num_regs);
    printf("%d\n", verify_sorted_array(regs, num_regs));
    fclose(fr);
    free(regs);
    return 0;
}
