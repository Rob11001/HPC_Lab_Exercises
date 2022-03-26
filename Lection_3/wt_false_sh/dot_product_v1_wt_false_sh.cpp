#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <omp.h>

#define N_THREADS 4
#define PADDING 32

int main(int argc, char **argv) {
    int *a, *b, c = 0;
    int N;
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Size
    N = atoi(argv[1]);
    
    // Matrix allocation
    a = (int *) malloc(sizeof(int) * N);
    b = (int *) malloc(sizeof(int) * N);
    
    // srand initialization
	srand((unsigned int) time(0));
    
    #pragma omp parallel for num_threads(N_THREADS) 
	for (int i = 0; i < N; i++) {
		a[i] = rand() % 5 - 2;
		b[i] = rand() % 5 - 2;
	}
    
    // Begin time
    auto begin = high_resolution_clock::now();

    // dot product
    int s_sum[N_THREADS * PADDING] = {0};
    // MAP
    #pragma omp parallel for shared(s_sum) num_threads(N_THREADS) 
    for(int i = 0; i < N; i++) {
        #ifdef DEBUG
            printf("Thread %d- i: %d\n", omp_get_thread_num(), i);
        #endif
        s_sum[omp_get_thread_num() * PADDING] += a[i] * b[i];
    }

    // REDUCE
    c = 0;
    for(int i = 0; i < N_THREADS; i++)
        c += s_sum[i * PADDING];
   
    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    if (N < 20) {
        for(int i = 0; i < N; i++)
            printf("a[%d]=%d ", i, a[i]);
        printf("\n");
        for(int i = 0; i < N; i++)
            printf("b[%d]=%d ", i, b[i]);
        printf("\n");
        printf("c=%d\n", c);
        printf("\n");
	}
    printf("%f", time);

    free(a);
    free(b);

    return 0;
}
