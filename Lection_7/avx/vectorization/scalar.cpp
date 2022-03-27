#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

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

    unsigned char *img_in1 = static_cast<unsigned char *> (malloc(sizeof(unsigned char) * SIZE));
    unsigned char *img_in2 = static_cast<unsigned char *> (malloc(sizeof(unsigned char) * SIZE)); 
    unsigned char *img_out = static_cast<unsigned char *> (malloc(sizeof(unsigned char) * SIZE));

    // srand initialization
	srand((unsigned int) time(0));

    // … initialize img_in1 and img_in2
    for(int i = 0; i < SIZE; i++) {
        img_in1[i] = i % 255;
        img_in2[i] = 0;
    }
    
    // Begin time
    auto begin = high_resolution_clock::now();

    for (int i = 0; i < SIZE; i++){
        img_out[i] = img_in1[i];
        if (img_in1[i] > 200)
            img_out[i] = img_in2[i];
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