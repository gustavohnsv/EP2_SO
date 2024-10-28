#define LIMIT 100000                // limite do vetor
#define MAX_THREADS 8               // máximo de threads
#define THRESHOLD (1 << 10)         // máximo do subarray

typedef struct{
    int* arr;       // vetor que está sendo utilizado
    int init;       // começo daquele subarray
    int end;        // fim daquele subarray
} thread_data;

void* merge(int* arr, int init, int mid, int end);
void* mergesort(int* arr, int init, int end);
void* __mergesort_threaded__(void* arg);
void* __mergesort_with_thread__(int* arr, int init, int end);