#include "psort14713982.h"

void print_error_and_exit(char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int open_file(char* filename, int flags, mode_t mode) {
    int fd = open(filename, flags, mode);
    if (fd == -1) {
        print_error_and_exit("Erro ao abrir o arquivo!");
    }
    return fd;
}

void* map_file(int fd, size_t size) {
    void* data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        print_error_and_exit("Erro ao mapear memória!");
    }
    return data;
}

void write_file(char* filename, void* data, size_t size) {
    int fd = open_file(filename, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (write(fd, data, size) == -1) {
        close(fd);
        print_error_and_exit("Erro ao escrever no arquivo de saída!");
    }
    fsync(fd);
    close(fd);
}

void* print_array(reg* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("[%03d°] - Chave: %u\n", i+1, arr[i].key);
        /* parte dos dados
        for (int k = 0; k < REG_DATA_BUFFER; k++) {
            printf("%02x", (unsigned char) regs[i].data[k]);
        }
        printf("\n");
        */
    }
    return NULL;
}

void verify_range(reg* arr, int start, int end, const char* msg) { // função usada apenas durante desenvolvimento
    printf("Verificando %s (índices %d até %d):\n", msg, start, end);
    for (int i = start; i < end; i++) {
        if (arr[i].key > arr[i+1].key) {
            printf("Erro no índice %d: %u > %u\n", i, arr[i].key, arr[i+1].key);
        }
    }
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
    int i = 0, j = 0, k = 0;
    while (i < arr1_elements && j < arr2_elements) {
        if (arr[init+i].key > arr[mid+1+j].key) {
            arr_temp[k] = arr[mid+1+j];
            j++;
        } else {
            arr_temp[k] = arr[init+i];
            i++;
        }
        k++;
    }
    while (i < arr1_elements) {
        arr_temp[k] =  arr[init+i];
        i++;
        k++;
    }
    while (j < arr2_elements) {
        arr_temp[k] = arr[mid+1+j];
        j++;
        k++;
    }
    for (int i = init; i <= end; i++) {
        arr[i] = arr_temp[i - init];
    }
    free(arr_temp);
    return NULL;
}

void* mergesort(reg* arr, int init, int end) { // função usada apenas durante desenvolvimento/testes
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
        *data = (thread_data){
            arr, 
            init + (i * chunk_size), 
            (i == nthreads - 1) ? end : (init + (i + 1) * chunk_size - 1)
        };       
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
        fprintf(stderr, "Uso: ./%s <arquivo_entrada> <arquivo_saída> <num_threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char* read_filename = argv[1];
    char* write_filename = argv[2];
    int threads = atoi(argv[3]);
    if (threads < 1) {
        fprintf(stderr, "Insira um número maior do que 0 para as threads!\n");
        exit(EXIT_FAILURE);
    }
    int fd = open(read_filename, O_RDONLY);
    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        print_error_and_exit("Erro ao obter informações sobre o arquivo!");
    }
    size_t file_size = st.st_size;
    void* read_filedata = map_file(fd, file_size);
    close(fd);
    int num_regs = file_size / REG_SIZE;
    reg* regs = (reg*) malloc(file_size);
    if (!regs) {
        print_error_and_exit("Erro ao alocar memória!");
    }
    memcpy(regs, read_filedata, file_size);
    while (verify_sorted_array(regs, num_regs) != 0) {
        mergesort_threaded(regs, 0, num_regs-1, threads);
    }
    write_file(write_filename, regs, file_size);
    if (munmap(read_filedata, file_size) == -1) {
        close(fd);
        print_error_and_exit("Erro ao desmapear memória!");
    }
    free(regs);
}