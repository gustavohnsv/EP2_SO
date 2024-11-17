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

void* map_file(int fd, size_t size, int flag) {
    int type;
    flag == 1 ? (type = MAP_PRIVATE) : (type = MAP_SHARED);
    void* data = mmap(NULL, size, PROT_READ | PROT_WRITE, type, fd, 0);
    if (data == MAP_FAILED) {
        print_error_and_exit("Erro ao mapear memória!");
    }
    return data;
}

void write_file(char* filename, void* data, size_t size) {
    int fd = open_file(filename, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (ftruncate(fd, size) != 0) {
        close(fd);
        print_error_and_exit("Erro ao tentar truncar o tamanho do arquivo");
    }
    void* write_data = map_file(fd, size, 0);
    memcpy(write_data, data, size);
    msync(write_data, size, MS_SYNC);
    if (munmap(write_data, size) == -1) {
        close(fd);
        print_error_and_exit("Erro ao desmapear memória!");
    }
    close(fd);
}

void print_array(reg* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("[%03d°] - Chave: %u\n", i+1, arr[i].key);
    }
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

void data_to_pointer(reg_w_data* arr1, reg* arr2, int size) {
    for (int i = 0; i < size; i++) {
        arr2[i].key = arr1[i].key;
        arr2[i].data = (reg_data*) &arr1[i].data;
    }
}

void pointer_to_data(reg* arr1, reg_w_data* arr2, int size) {
    for (int i = 0; i < size; i++) {
        arr2[i].key = arr1[i].key;
        memcpy(arr2[i].data, arr1[i].data->data, sizeof(arr2[i].data));
    }
}

void merge(reg* arr, reg* arr_temp, int init, int mid, int end) {
    int arr1_elements = mid - init + 1;
    int arr2_elements = end - mid;
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
}

int get_max(reg* arr, int init, int end) {
    int max = arr[init].key;
    for (int i = init+1; i < end; i++) {
        if (arr[i].key > max) {
            max = arr[i].key;
        }
    }
    return max;
}

void csort(reg* arr, reg* arr_temp, int size, int byte_index, int init, int end) {
    int count[256];
    memset(count, 0, sizeof(count));
    int shift = byte_index * 8;
    for (int i = init; i <= end; i++) {
        int byte_value = (arr[i].key >> shift) & 0xFF;
        count[byte_value]++;
    }
    for (int i = 1; i < 256; i++) {
        count[i] += count[i - 1];
    }
    for (int i = end; i >= init; i--) {
        int byte_value = (arr[i].key >> (byte_index * 8)) & 0xFF;
        arr_temp[count[byte_value] - 1] = arr[i];
        count[byte_value]--;
    }
    for (int i = init; i <= end; i++) {
        arr[i] = arr_temp[i - init];
    }
}

void rsort(reg* arr, int size, int init, int end) {
    reg* arr_temp = (reg*) malloc(sizeof(reg) * size);
    if (!arr_temp) {
        print_error_and_exit("Erro ao alocar memória!");
    }
    int num_bytes = sizeof(arr[0].key) / sizeof(unsigned char);
    for (int byte_index = 0; byte_index < num_bytes; byte_index++) {
        csort(arr, arr_temp, size, byte_index, init, end);
    }
    free(arr_temp);
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
        rsort(data->arr, data->end-data->init+1, data->init, data->end);
        //qsort(data->arr+data->init, (data->end-data->init+1), sizeof(reg), compare);
    } else {
        isort(data->arr, data->init, data->end);
    }
    return NULL;
}

void mergesort_threaded(reg* arr, int init, int end, int nthreads) {
    pthread_t thread[nthreads];
    thread_data* datas = (thread_data*) malloc(sizeof(thread_data)*nthreads);
    if (!datas) {
        print_error_and_exit("Erro ao alocar memória!");
    }
    if (nthreads > (end - init + 1)) {
        nthreads = (end - init + 1);
    }
    int chunk_size = (end - init + 1) / nthreads;
    if (chunk_size < 1) chunk_size = 1;   
    for (int i = 0; i < nthreads; i++) {
        datas[i].arr = arr;
        datas[i].init = init + (i * chunk_size);
        datas[i].end = (i == nthreads - 1) ? end : (init + (i + 1) * chunk_size - 1);    
        if (pthread_create(&thread[i], NULL, sort_option, (thread_data*)&datas[i]) != 0) {
            for (int j = 0; j < i; j--) {
                free(datas);
            }
            print_error_and_exit("Erro ao alocar uma thread!");
        }
    }
    for (int i = 0; i < nthreads; i++) {
        pthread_join(thread[i], NULL);
    }
    free(datas);
    reg* arr_temp = (reg*) malloc(sizeof(reg)*(end-init+1));
    if (!arr_temp) {
        print_error_and_exit("Erro ao alocar memória!");
    }
    for (int size = chunk_size; size <= (end - init); size = size * 2) {
        for (int left = init; left <= end - size; left += size * 2) {
            int mid = left + size - 1;
            int right = (left + (2 * size) - 1 <= end) ? left + (2 * size) - 1 : end;
            merge(arr, arr_temp, left, mid, right);
        }
    }
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
    if (file_size % REG_SIZE != 0) {
        print_error_and_exit("Tamanho de arquivo incompatível com REG_SIZE!");
    }
    void* read_filedata = map_file(fd, file_size, 1);
    close(fd);
    int num_regs = file_size / REG_SIZE;
    reg* regs = (reg*) malloc(sizeof(reg)*num_regs);
    if (!regs) {
        print_error_and_exit("Erro ao alocar memória!");
    }
    reg_w_data* regs_w_data = (reg_w_data*) malloc(sizeof(reg_w_data)*num_regs);
    if (!regs_w_data) {
        print_error_and_exit("Erro ao alocar memória!");
    }
    regs_w_data = (reg_w_data*) read_filedata;
    data_to_pointer(regs_w_data, regs, num_regs);
    mergesort_threaded(regs, 0, num_regs-1, threads);
    if (verify_sorted_array(regs, num_regs) != 0) {
        print_error_and_exit("Array não foi ordenado corretamente!");
    } else {
        printf("Array ordenado corretamente!\n");
    }
    pointer_to_data(regs, regs_w_data, num_regs);
    write_file(write_filename, regs_w_data, file_size);
    if (munmap(read_filedata, file_size) == -1) {
        close(fd);
        print_error_and_exit("Erro ao desmapear memória!");
    }
}