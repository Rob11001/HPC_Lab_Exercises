#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

//#define DEBUG
#define N_THREADS 4

double f(double x);
double calc_pi(int n);

int main(int argc, char **argv) {
    int N;
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

    // Begin time
    auto begin = high_resolution_clock::now();

    double pi = calc_pi(N);

    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    #ifdef DEBUG
    printf("pi: %f\n", pi);
    printf("Time: %f\n", time);
    #endif

    #ifndef DEBUG
    printf("%f", time);
    #endif
    
    return 0;
}


double f(double x) {
    return (4.0 / (1.0 + x*x));
}

double calc_pi(int n) {
    const double fH = 1.0 / (double) n;
    double fSum = 0.0;
    double fX;

    #pragma omp parallel for default(none) private(fX) shared(n, fH) reduction(+:fSum) num_threads(N_THREADS)
    for(int i = 0; i < n; i++) {
        fX = fH * ((double)i + 0.5);
        fSum += f(fX);
    }

    return fH * fSum;
}