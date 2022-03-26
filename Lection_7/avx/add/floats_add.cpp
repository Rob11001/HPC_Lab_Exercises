#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

/**
 * @brief 
 * 
 * Adds two array of N floating point numbers
 */

//#define DEBUG 

int main(int argc, char **argv) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    int N;
    float *a, *b, *c;
    
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Size
    N = atoi(argv[1]);

    // Array allocation
    a = (float *) malloc(sizeof(float) * N);
    b = (float *) malloc(sizeof(float) * N);
    c = (float *) malloc(sizeof(float) * N);
    
    // srand initialization
	srand((unsigned int) time(0));
    
    // Array initialization
    for(int i = 0; i < N; i++) {
        a[i] = rand() % 10;
        b[i] = rand() % 10;
        c[i] = 0;
    }

    // Begin time
    auto begin = high_resolution_clock::now();

    for(int i = 0; i < N; i++)
        c[i] = a[i] + b[i];

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
        
        for (int i = 0; i < N; i++)
            printf("c[%d]=%.2f ", i, c[i]);
        printf("\n");

        printf("Time: %f\n", time);
    #endif

    #ifndef DEBUG
        printf("%f", time);
    #endif

    free(a);
    free(b);
    free(c);

    return 0;
}
