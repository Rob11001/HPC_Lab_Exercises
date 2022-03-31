#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

/**
 * @brief 
 * Complex number multiplication using SoA layout and intrinsics API
 * Remind: (a + bi) * (c + di) = (ac - bd) + (ad + bc)i
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
    
    // Real and immaginary part of the first array
    double *a = static_cast<double *>(aligned_alloc(32, (N) * sizeof(double)));
    double *b = static_cast<double *>(aligned_alloc(32, (N) * sizeof(double)));
    // Real and immaginary part of the second array
    double *c = static_cast<double *>(aligned_alloc(32, (N) * sizeof(double)));
    double *d = static_cast<double *>(aligned_alloc(32, (N) * sizeof(double)));
    
    // Real and immaginary part of the result array
    double *real = static_cast<double *>(aligned_alloc(32, (N) * sizeof(double)));
    double *imm = static_cast<double *>(aligned_alloc(32, (N) * sizeof(double)));

    __m256d neg = _mm256_set_pd(-1.0, 1.0, -1.0, 1.0);

    // srand initialization
	srand((unsigned int) time(0));

    // Array initialization
    for(int i = 0; i < N; i++) {
        a[i] = rand() % 10;
        b[i] = rand() % 10;
        c[i] = rand() % 10;
        d[i] = rand() % 10;
    }

    // Begin time
    auto begin = high_resolution_clock::now();
    
    for(int i = 0; i < N; i += 4) {
        // load 4 doubles from A and B
        __m256d a_avx = _mm256_load_pd(&a[i]);
        __m256d b_avx = _mm256_load_pd(&b[i]);
        __m256d c_avx = _mm256_load_pd(&c[i]);
        __m256d d_avx = _mm256_load_pd(&d[i]);
        
        // Creates vector a*c
        __m256d ac_avx = _mm256_mul_pd(a_avx, c_avx);
        // Creates vector b*d
        __m256d bd_avx = _mm256_mul_pd(b_avx, d_avx);
        // Creates vector a*d
        __m256d ad_avx = _mm256_mul_pd(a_avx, d_avx);
        // Creates vector b*c
        __m256d bc_avx = _mm256_mul_pd(b_avx, c_avx);
        
        // Creates vector ac - bd
        ac_avx = _mm256_sub_pd(ac_avx, bd_avx);
        // Creates vector ad + bc
        ad_avx = _mm256_add_pd(ad_avx, bc_avx);

        _mm256_stream_pd(&real[i], ac_avx);
        _mm256_stream_pd(&imm[i], ad_avx);
    }

    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    #ifdef DEBUG
        for (int i = 0; i < N; i++)
            printf("A[%d]= %.2f + %.2f * i ", i, a[i], b[i]);
        printf("\n");

        for (int i = 0; i < N; i++)
            printf("B[%d]= %.2f + %.2f * i ", i, c[i], d[i]);
        printf("\n");

        for (int i = 0; i < N; i++)
            printf("C[%d]= %.2f + %.2f * i ", i, real[i], imm[i]);
        printf("\n");
        
        printf("Time: %f\n", time);
    #endif

    #ifndef DEBUG
        printf("%f", time);
    #endif

    free(a);
    free(b);
    free(c);
    free(d);
    free(real);
    free(imm);
}