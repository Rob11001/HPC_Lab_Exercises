#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

/**
 * @brief 
 * 
 * Histogram counting: 
 * 
 * Given an array of N random integers in [1,100], implement a parallel algorithm that counts how many elements fall in each bucket: 
    - [1,10]
    - [11, 20]
    - [21,30] 
    - ...
    - [91,100]

    Using #pragma omp atomic
 */

//#define DEBUG 
#define PADDING 16
#define NUM_THREADS 16

int main(int argc, char **argv) {
    int N;
    int counts[10 * PADDING] = {0};

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
    int *random_numbers = (int *) malloc(sizeof(int) * N);

    // srand initialization
	srand((unsigned int) time(0));
    
    for(int i = 0; i < N; i++)
        random_numbers[i] = rand() % 100 + 1;

    // Begin time
    auto begin = high_resolution_clock::now();
    int index;
    #pragma omp parallel for default(none) shared(random_numbers, counts, N) private(index) num_threads(4)
    for(int i = 0; i < N; i++) {
        index = (random_numbers[i] % 10 == 0) ? random_numbers[i] / 10 - 1 : random_numbers[i] / 10;
        
        #pragma omp atomic
        counts[index * PADDING] += 1;
    }

    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    #ifdef DEBUG
        for (int i = 0; i < N; i++)
            printf("A[%d]=%d ", i, random_numbers[i]);
        printf("\n");
        for (int i = 0; i < 10; i++)
            printf("counts[%d]=%d ", i, counts[i * PADDING]);
        printf("\n");
        printf("Time: %f\n", time);
    #endif

    #ifndef DEBUG
        printf("%f", time);
    #endif

    return 0;
}
