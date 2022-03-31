#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

/**
 * @brief 
 * Complex number multiplication using AoS layout and intrinsics API
 * Remind: (a + bi) * (c + di) = (ac - bd) + (ad + bc)i
 * 
 */

//#define DEBUG

int main(int argc, char **argv) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    int N;
    
    if(argc != 2) {
         fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return EXIT_FAILURE;
    }

    N = atoi(argv[1]);
    
    double *A = static_cast<double *>(aligned_alloc(32, 2 * N * sizeof(double)));
    double *B = static_cast<double *>(aligned_alloc(32, 2 * N * sizeof(double)));
    double *C = static_cast<double *>(aligned_alloc(32, 2 * N * sizeof(double)));
    __m256d neg = _mm256_set_pd(-1.0, 1.0, -1.0, 1.0);

    // srand initialization
	srand((unsigned int) time(0));

    // Array initialization
    for(int i = 0; i < 2 * N; i++) {
        A[i] = rand() % 10;
        B[i] = rand() % 10;
        C[i] = 0;
    }

    // Begin time
    auto begin = high_resolution_clock::now();
    
    for(int i = 0; i < 2 * N; i += 4) {
        // load 4 doubles from A and B
        __m256d a_avx = _mm256_load_pd(&A[i]);
        __m256d b_avx = _mm256_load_pd(&B[i]);
        
        // Creates vector a*b
        __m256d c_avx = _mm256_mul_pd(a_avx, b_avx);
        // Permutes b and multiplies it with neg
        b_avx = _mm256_permute_pd(b_avx, 0x5); // 0x5 0b0101
        b_avx = _mm256_mul_pd(b_avx, neg);
        // Creates vector a * the new b
        b_avx = _mm256_mul_pd(a_avx, b_avx);

        // Horizontal sub to calculate the product
        a_avx = _mm256_hsub_pd(c_avx, b_avx);

        _mm256_stream_pd(&C[i], a_avx);
    }

    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    #ifdef DEBUG
        for (int i = 0; i < 2 * N; i += 2)
            printf("A[%d]= %.2f + %.2f * i ", i/2, A[i], A[i + 1]);
        printf("\n");
        
        for (int i = 0; i < 2 * N; i += 2)
            printf("B[%d]= %.2f + %.2f * i ", i/2, B[i], B[i + 1]);
        printf("\n");
        
        for (int i = 0; i < 2 * N; i += 2)
            printf("C[%d]= %.2f + %.2f * i ", i/2, C[i], C[i + 1]);
        printf("\n");

        printf("Time: %f\n", time);
    #endif

    #ifndef DEBUG
        printf("%f", time);
    #endif

    free(A);
    free(B);
    free(C);
}