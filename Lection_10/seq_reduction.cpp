#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

/**
 * @brief 
 * 
 * Sequential reduction using "+" operator
 */

//#define DEBUG 

int main(int argc, char **argv) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    int N;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Size
    N = atoi(argv[1]);
    float *array = static_cast<float *>(malloc(sizeof(float) * N));
    // srand initialization
	srand((unsigned int) time(0));
    
    for(int i = 0; i < N; i++)
       array[i] = rand() % 10;

    // Begin time
    auto begin = high_resolution_clock::now();

    float acc = array[0];
    for(int i = 1; i < N; i++) {
        acc += array[i];
    }

    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    #ifdef DEBUG
        for (int i = 0; i < N; i++)
            printf("A[%d]=%f ", i, array[i]);
        printf("\n");
        printf("Reduced value: %f\n", acc);
        printf("Time: %f\n", time);
    #endif

    #ifndef DEBUG
        printf("%f, p", time);
    #endif

    free(array);

    return 0;
}
