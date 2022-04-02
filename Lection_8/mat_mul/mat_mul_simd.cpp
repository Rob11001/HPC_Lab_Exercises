#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <omp.h>

/**
 * @brief Matrix multiplication using OpenMP and #pragma simd
 */

//#define DEBUG

int main(int argc, char **argv) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    int N, num_threads;
    int *A, *B, *C; 

    if(argc != 3) {
        fprintf(stderr, "Usage: %s <N> <NUM_THREADS> \n", argv[0]);
        return EXIT_FAILURE;
    }

    // Size
    N = atoi(argv[1]);
    num_threads = atoi(argv[2]);

    A = (int *) aligned_alloc(32, sizeof(int) * N * N);
    B = (int *) aligned_alloc(32, sizeof(int) * N * N);
    C = (int *) aligned_alloc(32, sizeof(int) * N * N);

    // srand initialization
	srand((unsigned int) time(0));

    for(int i = 0; i < N * N; i++) {
        A[i] = 1;
        B[i] = 1;
        C[i] = 0;
    }

    // Begin time
    auto begin = high_resolution_clock::now();

    // Multiplication 
    #pragma omp parallel for simd num_threads(num_threads) aligned(A:32) aligned(B:32) aligned (C:32)
    for(int i = 0; i < N; i++)
        for (int k = 0; k < N; k++)
            for(int j = 0; j < N; j++)
                C[i * N + j] += A[i * N + k] * B[k * N + j];     
    
    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    #ifdef DEBUG
        if(N < 32) {
            for(int i = 0; i < N * N; i++) 
                printf("a[%d][%d] = %d ", i / N, i % N, A[i]);
            printf("\n");
            
            for(int i = 0; i < N * N; i++) 
                printf("b[%d][%d] = %d ", i / N, i % N, B[i]);
            printf("\n");

            for(int i = 0; i < N * N; i++) 
                printf("c[%d][%d] = %d ", i / N, i % N, C[i]);
            printf("\n");
        } else {
            for(int i = 0; i < N * N; i++) 
                if(C[i] != N) {
                    printf("Errore : c[%d] = %d\n", i, C[i]);
                    break;
                }
        }
        printf("Time: %f\n", time);
    #endif

    #ifndef DEBUG
        printf("%f", time);
    #endif

    free(A);
    free(B);
    free(C);

    return 0;
}
