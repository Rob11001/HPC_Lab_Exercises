#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <immintrin.h>

/**
 * @brief 
 * Prefix sum (cumulative sum) of an array using __m128i intrinsics
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

    uint32_t *array = static_cast<u_int32_t *>(aligned_alloc(32, N * sizeof(uint32_t)));
    u_int32_t *output = static_cast<u_int32_t *>(aligned_alloc(32, N * sizeof(uint32_t)));

    // srand initialization
	srand((unsigned int) time(0));

    // Array initialization
    for(int i = 0; i < N; i++) {
        array[i] = rand() % 10;
    }

    // Begin time
    auto begin = high_resolution_clock::now();

    uint32_t accumulator = 0;
    for(size_t i = 0; i < N; i += 4) {
        __m128i vec = _mm_load_si128((__m128i *) &array[i]);
        vec = _mm_shuffle_epi32(vec, 0b00011011); // Reverse order

        // Prefix sum 
        __m128i tmp = _mm_srli_si128(vec, 4);
        vec = _mm_add_epi32(vec, tmp);

        tmp = _mm_srli_si128(vec, 8);
        vec = _mm_add_epi32(vec, tmp);

        // Update with the prev accumulator
        tmp = _mm_set_epi32(accumulator, accumulator, accumulator, accumulator);
        vec = _mm_add_epi32(vec, tmp);
        
        accumulator = ((u_int32_t *) &vec)[0]; // Updates for the next iteration

        // Need to store, but how? Yeah, it works
        vec = _mm_shuffle_epi32(vec, 0b00011011);
        _mm_store_si128((__m128i *) &output[i], vec);
    }

    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    #ifdef DEBUG
        for (int i = 0; i < N; i++)
            printf("Array[%d]= %d ", i, array[i]);
        printf("\n");       

        for (int i = 0; i < N; i++)
            printf("Output[%d]= %d ", i, output[i]);
        printf("\n"); 

        printf("Time: %f\n", time);
    #endif

    #ifndef DEBUG
        printf("%f", time);
    #endif

    free(array);
    free(output);
}