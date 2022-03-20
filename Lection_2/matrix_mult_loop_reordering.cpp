#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

int main(int argc, char **argv) {
    int *a, *b, *c = 0;
    int N, dim; 
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
    dim = N * N;
    
    // Matrix allocation
    a = (int *) malloc(sizeof(int) * dim);
    b = (int *) malloc(sizeof(int) * dim);
    c = (int *) malloc(sizeof(int) * dim);
    
    // srand initialization
	srand((unsigned int) time(0));
    
	for (int i = 0; i < dim; i++) {
		a[i] = rand() % 5 - 2;
		b[i] = rand() % 5 - 2;
        c[i] = 0;
	}
    
    // Begin time
    auto begin = high_resolution_clock::now();

    // Multiplication
    for(int i = 0; i < N; i++)
        for (int k = 0; k < N; k++) {
            for(int j = 0; j < N; j++)
                c[i * N + j] += a[i * N + k] * b[k * N + j];     
        }

    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    if (N < 20) {
        for(int i = 0; i < dim; i++)
            printf("a[%d]=%d ", i, a[i]);
        printf("\n");
        for(int i = 0; i < dim; i++)
            printf("b[%d]=%d ", i, b[i]);
        printf("\n");
        for(int i = 0; i < dim; i++)
            printf("c[%d]=%d ", i, c[i]);
        printf("\n");
	}
    printf("Matrix dimension: %d\n", N);
    printf("Time: %f\n", time);

    free(a);
    free(b);
    free(c);

    return 0;
}
