#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

/**
 * @brief 
 * Prefix sum (cumulative sum) of an array
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

    uint32_t *array = static_cast<u_int32_t *>(malloc(N * sizeof(uint32_t)));
    u_int32_t *output = static_cast<u_int32_t *>(malloc(N * sizeof(uint32_t)));

    // srand initialization
	srand((unsigned int) time(0));

    // Array initialization
    for(int i = 0; i < N; i++) {
        array[i] = rand() % 10;
    }

    // Begin time
    auto begin = high_resolution_clock::now();

    uint32_t accumulator = 0;
    for(size_t i = 0; i < N; i++) {
        accumulator += array[i];
        output[i] = accumulator;
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