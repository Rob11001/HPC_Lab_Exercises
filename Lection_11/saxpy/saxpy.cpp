#include <iostream>
#include <CL/cl.h>
#include <CL/sycl.hpp>

//#define DEBUG

using namespace sycl;

/**
 * @brief saxpy on GPU using SYCL
 */

int main(int argc, char **argv) {
    int N;
    
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <N>" << std::endl;

        return EXIT_FAILURE;
    }
    N = atoi(argv[1]);

    // Allocate memory for the three vectors
    float *z = static_cast<float *>(malloc(sizeof(float) * N));
    float *x = static_cast<float *>(malloc(sizeof(float) * N));
    float *y = static_cast<float *>(malloc(sizeof(float) * N));

    // Initialization
    for(int i {0}; i < N; i++)
        x[i] = y[i] = 1.0f;
     float alpha = 2.0f;

    // Get the queue for the gpu_device
    queue myQueue { gpu_selector() };
    
    // Use of RAII
    {
        // Wrap out vectors in buffers
        buffer<float, 1> X {x, N};
        buffer<float, 1> Y {y, N};
        buffer<float, 1> Z {z, N};

        // Create a command group to issue commands to the queue
        myQueue.submit([&] (handler& cgh) {
            // Request access to the buffers
            accessor x_buf {X, cgh, read_only};
            accessor y_buf {Y, cgh, read_only};
            accessor z_buf {Z, cgh, write_only, no_init};
         
            // Enqueue a parallel_for task with N work-items
            cgh.parallel_for(N, [=] (id<1> idx) {
               z_buf[idx] = alpha * x_buf[idx] + y_buf[idx];
            }); // End of the kernel function
          
        }); // End of our commands for this queue
    } // End of scope

    
    #ifndef DEBUG
        // Print result
        for(int i {0}; i < N; i++)
            if(z[i] != 3) 
                std::cout << "Error: z[" << i << "] = " << z[i] << std::endl; 
        std::cout << "Finished" << std::endl;
    #endif

    #ifdef DEBUG
        for(int i {0}; i < N; i++)
            std::cout << "z[" << i << "] = " << z[i] << std::endl; 
    #endif

    // Deallocate memory
    free(x);
    free(y);
    free(z);

    return 0; 
}