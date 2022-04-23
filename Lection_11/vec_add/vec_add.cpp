#include <iostream>
#include <CL/cl.h>
#include <CL/sycl.hpp>

//#define DEBUG

using namespace sycl;

/**
 * @brief Vector addition using SYCL on GPU device
 */

int main(int argc, char **argv) {
    int N;
    
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <N>" << std::endl;

        return EXIT_FAILURE;
    }
    N = atoi(argv[1]);

    // Allocate memory for the three vectors
    int *a = static_cast<int *>(malloc(sizeof(int) * N));
    int *b = static_cast<int *>(malloc(sizeof(int) * N));
    int *c = static_cast<int *>(malloc(sizeof(int) * N));

    // Initialization
    for(int i {0}; i < N; i++)
        a[i] = b[i] = 1;

    // Get the queue for the gpu_device
    queue myQueue { gpu_selector() };
    
    // Use of RAII
    {
        // Wrap out vectors in buffers
        buffer<int, 1> A {a, N};
        buffer<int, 1> B {b, N};
        buffer<int, 1> C {c, N};

        // Create a command group to issue commands to the queue
        myQueue.submit([&] (handler& cgh) {
            // Request access to the buffers
            accessor a_buf {A, cgh, read_only};
            accessor b_buf {B, cgh, read_only};
            accessor c_buf {C, cgh, write_only, no_init};
         
            // Enqueue a parallel_for task with N work-items
            cgh.parallel_for(N, [=] (id<1> idx) {
                c_buf[idx] = a_buf[idx] + b_buf[idx];
            }); // End of the kernel function
          
        }); // End of our commands for this queue
    } // End of scope

    
    #ifndef DEBUG
        // Print result
        for(int i {0}; i < N; i++)
            if(c[i] != 2) 
                std::cout << "Error: c[" << i << "] = " << c[i] << std::endl; 
    #endif

    #ifdef DEBUG
        for(int i {0}; i < N; i++)
            std::cout << "c[" << i << "] = " << c[i] << std::endl; 
    #endif

    // Deallocate memory
    free(a);
    free(b);
    free(c);

    return 0; 
}