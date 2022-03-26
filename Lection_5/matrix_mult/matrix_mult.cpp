#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

//#define DEBUG 

void matrix_mul(int *A, int begin_ra, int end_ra, int begin_ca, int end_ca, int *B, int begin_rb, int end_rb, int begin_cb, int end_cb, int *C,  int begin_rc, int end_rc, int begin_cc, int end_cc, int N);

int main(int argc, char **argv) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    int N;
    int *A, *B, *C; 

    if(argc < 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Size
    N = atoi(argv[1]);
    A = (int *) malloc(sizeof(int) * N * N);
    B = (int *) malloc(sizeof(int) * N * N);
    C = (int *) malloc(sizeof(int) * N * N);

    // srand initialization
	srand((unsigned int) time(0));

    for(int i = 0; i < N * N; i++) {
        A[i] = 1;
        B[i] = 1;
        C[i] = 0;
    }

    // Begin time
    auto begin = high_resolution_clock::now();

    matrix_mul(A, 0, N, 0, N, B, 0, N, 0, N, C, 0, N, 0, N, N);
    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    #ifdef DEBUG
        for(int i = 0; i < N * N; i++) 
            printf("a[%d][%d] = %d ", i / N, i % N, A[i]);
        printf("\n");
        
        for(int i = 0; i < N * N; i++) 
            printf("b[%d][%d] = %d ", i / N, i % N, B[i]);
        printf("\n");

        for(int i = 0; i < N * N; i++) 
            printf("c[%d][%d] = %d ", i / N, i % N, C[i]);
        printf("\n");

        printf("Time: %f\n", time);
    #endif

    #ifndef DEBUG
        printf("%f", time);
    #endif

    return 0;
}


// Ipotesi: matrici quadrate

void matrix_mul(int *A, int begin_ra, int end_ra, int begin_ca, int end_ca, int *B, int begin_rb, int end_rb, int begin_cb, int end_cb, int *C,  int begin_rc, int end_rc, int begin_cc, int end_cc, int N) {
    // Base case
    if(end_rc - begin_rc <= 256 && end_cc - begin_cc <= 256) {
        // Risolvi sequenzialmente
        A += begin_ra * N + begin_ca;
        B += begin_rb * N + begin_cb;
        C += begin_rc * N + begin_cc;
        int end_r = end_rc - begin_rc;
        int end_c = end_cc - begin_cc;
        
        for(int i = 0; i < end_r; i++)
            for (int k = 0; k < end_r; k++) 
                for(int j = 0; j < end_c; j++)
                    C[i * N + j] += A[i * N + k] * B[k * N + j];     
        
        return;
    }


    int med_ra, med_ca, med_rb, med_cb, med_rc, med_cc;
    med_ra = (begin_ra + end_ra) / 2;
    med_ca = (begin_ca + end_ca) / 2;
    med_rb = (begin_rb + end_rb) / 2;
    med_cb = (begin_cb + end_cb) / 2;
    med_rc = (begin_rc + end_rc) / 2;
    med_cc = (begin_cc + end_cc) / 2;

    // A_00 * B_00
    matrix_mul(A, begin_ra, med_ra, begin_ca, med_ca, B, begin_rb, med_rb, begin_cb, med_cb, C, begin_rc, med_rc, begin_cc, med_cc, N);
    // A_00 * B_01
    matrix_mul(A, begin_ra, med_ra, begin_ca, med_ca, B, begin_rb, med_rb, med_cb, end_cb, C, begin_rc, med_rc, med_cc, end_cc, N);
    // A_10 * B_00
    matrix_mul(A, med_ra, end_ra, begin_ca, med_ca, B, begin_rb, med_rb, begin_cb, med_cb, C, med_rc, end_rc, begin_cc, med_cc, N);
    // A_10 * B_01
    matrix_mul(A, med_ra, end_ra, begin_ca, med_ca, B, begin_rb, med_rb, med_cb, end_cb, C, med_rc, end_rc, med_cc, end_cc, N);
    
    // A_01 * B_10
    matrix_mul(A, begin_ra, med_ra, med_ca, end_ca, B, med_rb, end_rb, begin_cb, med_cb, C, begin_rc, med_rc, begin_cc, med_cc, N);
    // A_01 * B_11
    matrix_mul(A, begin_ra, med_ra, med_ca, end_ca, B, med_rb, end_rb, med_cb, end_cb, C, begin_rc, med_rc, med_cc, end_cc, N);
    // A_11 * B_10
    matrix_mul(A, med_ra, end_ra, med_ca, end_ca, B, med_rb, end_rb, begin_cb, med_cb, C, med_rc, end_rc, begin_cc, med_cc, N);
    // A_11 * B_11
    matrix_mul(A, med_ra, end_ra, med_ca, end_ca, B, med_rb, end_rb, med_cb, end_cb, C, med_rc, end_rc, med_cc, end_cc, N);
    
}