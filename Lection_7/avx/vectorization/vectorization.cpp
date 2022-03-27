#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <immintrin.h>

/**
 * @brief 
 * 
 * A simple for to vectorize
 */

//#define DEBUG 

int main(int argc, char **argv) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Size
    int SIZE = atoi(argv[1]);

    unsigned char *img_in1 = static_cast<unsigned char *> (aligned_alloc(32, SIZE));
    unsigned char *img_in2 = static_cast<unsigned char *> (aligned_alloc(32, SIZE)); 
    unsigned char *img_out = static_cast<unsigned char *> (aligned_alloc(32, SIZE));
    unsigned char ch = static_cast<unsigned char> (200U);
    __m256i static_vector = _mm256_set_epi8(ch, ch, ch, ch, ch, ch, ch, ch,
        ch, ch, ch, ch, ch, ch, ch, ch,
        ch, ch, ch, ch, ch, ch, ch, ch,
        ch, ch, ch, ch, ch, ch, ch, ch);

    // srand initialization
	srand((unsigned int) time(0));

    // … initialize img_in1 and img_in2
    for(int i = 0; i < SIZE; i++) {
        img_in1[i] = static_cast<unsigned char>(198U + i);
        img_in2[i] = static_cast<unsigned char>(69U);
    }
 
    // Begin time
    auto begin = high_resolution_clock::now();

    for (int i = 0; i < SIZE; i += 32) {
        __m256i vector_1 = _mm256_stream_load_si256((__m256i *) (img_in1 + i));
        __m256i vector_2 = _mm256_stream_load_si256((__m256i *) (img_in2 + i));
        
        __m256i mask = _mm256_max_epu8(vector_1, static_vector); // take the max
        mask = _mm256_cmpeq_epi8(mask, static_vector); // if 200 is the max has 255, 0 otherwise

        __m256i result = _mm256_blendv_epi8(vector_2, vector_1, mask); // if the mask is 255 takes vector_1 value, otherwise vector_2
        _mm256_store_si256((__m256i *) (img_out + i), result);
    }

    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    // print results
    #ifdef DEBUG
        for (int i = 0; i < SIZE; i++)
            printf("img_in1[%d]=%d ", i, img_in1[i]);
        printf("\n");
        
        for (int i = 0; i < SIZE; i++)
            printf("img_in2[%d]=%d ", i, img_in2[i]);
        printf("\n");
        
        for (int i = 0; i < SIZE; i++)
            printf("img_out[%d]=%d ", i, img_out[i]);
        printf("\n");

        printf("Time: %f\n", time);
    #endif

    #ifndef DEBUG
        printf("%f", time);
    #endif
    
    
    free(img_in1);
    free(img_in2);
    free(img_out);

    return 0;
}