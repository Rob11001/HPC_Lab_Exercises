#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <immintrin.h>

/**
 * @brief 
 * 
 * Dot product using FMA and hadd
 */

//#define DEBUG 

int main(int argc, char **argv) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    int N;
    float *a, *b;
    float c = 0;
    
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Size
    N = atoi(argv[1]);

    // Array allocation
    a = (float *) aligned_alloc(32, sizeof(float) * N);
    b = (float *) aligned_alloc(32, sizeof(float) * N);
    
    // srand initialization
	srand((unsigned int) time(0));
    
    // Array initialization
    for(int i = 0; i < N; i++) {
        a[i] = 1;
        b[i] = 2;
    }

    // Zero vector
    __m256 zero_v = _mm256_setzero_ps();

    // Begin time
    auto begin = high_resolution_clock::now();

    for(int i = 0; i < N; i += 8) {
        // MAP
        __m256 a_v = _mm256_load_ps(&a[i]);
        __m256 b_v = _mm256_load_ps(&b[i]);
        __m256 tmp = _mm256_fmadd_ps(a_v, b_v, zero_v);
        // REDUCTION
        tmp = _mm256_hadd_ps(tmp, zero_v);
        tmp = _mm256_hadd_ps(tmp, zero_v);
        float *t = (float *) &tmp;
        c += t[0] + t[4];
    }

    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    #ifdef DEBUG
        for (int i = 0; i < N; i++)
            printf("a[%d]=%.2f ", i, a[i]);
        printf("\n");
        
        for (int i = 0; i < N; i++)
            printf("b[%d]=%.2f ", i, b[i]);
        printf("\n");
        
        printf("c=%.2f\n", c);

        printf("Time: %f\n", time);
    #endif

    #ifndef DEBUG
        printf("%f", time);
    #endif

    free(a);
    free(b);

    return 0;
}
