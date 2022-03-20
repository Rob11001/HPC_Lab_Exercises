#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

//#define DEBUG 

void mergesort(int *list, int begin, int end);
void merge(int *list, int begin, int med, int end);

int main(int argc, char **argv) {
    int N;
    int *list;
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
    list = (int *) malloc(sizeof(int) * N);

    // srand initialization
	srand((unsigned int) time(0));

    for(int i = 0; i < N; i++) 
        list[i] = rand() % 10 - 2;

    // Begin time
    auto begin = high_resolution_clock::now();

    mergesort(list, 0, N - 1);

    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    #ifdef DEBUG
        for(int i = 0; i < N; i++) 
            printf("a[%d] = %d, ", i, list[i]);
        printf("\n");
        printf("Time: %f\n", time);
    #endif

    #ifndef DEBUG
        printf("%f", time);
    #endif

    return 0;
}


void mergesort(int *list, int begin, int end) {
    if(begin >= end) return;

    int med = (begin + end) / 2;
    mergesort(list, begin, med);
    mergesort(list, med + 1, end);
    merge(list, begin, med, end);
}

void merge(int *list, int begin, int med, int end) {
    int *tmp = (int *) malloc(sizeof(int) * (end - begin + 1));
    int i, j, k;
    for(i = 0, j = begin, k = med + 1; j <= med && k <= end; i++) 
        if(list[j] < list[k]) {
            tmp[i] = list[j++];
        } else {
            tmp[i] = list[k++];
        }

    while(j <= med) 
        tmp[i++] = list[j++];

    while (k <= end)
        tmp[i++] = list[k++];
    
    for(i = begin, j = 0; i <= end; i++, j++) 
        list[i] = tmp[j];

    free(tmp);
}