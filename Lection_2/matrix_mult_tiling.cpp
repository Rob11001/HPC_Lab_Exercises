#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

#define S 32

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
    #pragma omp parallel for
    for(int ih = 0; ih < N; ih += S)
        for(int jh = 0; jh < N; jh += S)
            for(int kh = 0; kh < N; kh += S)
                for(int il = 0; il < S; il++)
                    for(int kl = 0; kl < S; kl++)
                        for(int jl = 0; jl < S; jl++)
                            c[(ih + il) * N + (jh + jl)] += a[(ih + il) * N + (kh + kl)] * b[(kh + kl) * N + (jh + jl)];    

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
