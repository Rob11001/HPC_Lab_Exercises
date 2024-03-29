#include <iostream>
#include <CL/cl.h>
#include <CL/sycl.hpp>

using namespace sycl;

int main() {
    int data[1024]; // Allocate data to be worked on
    // Create a default queue to enqueue work to the default device
    queue myQueue { default_selector() };
    // By wrapping all the SYCL work in a {} block, we ensure all SYCL tasks must
    // before exiting the block, because the destructor of resultBuf will wait
    {
        // Wrap out data variable in a buffer
        buffer<int, 1> resultBuf { data, range<1>{ 1024 } };

        // Create a command group to issue commands to the queue
        myQueue.submit([&] (handler& cgh) {
            // Request write access to the buffer without initialization
            accessor writeResult {resultBuf, cgh, write_only, no_init};
         
            // Enqueue a parallel_for task with 1024 work-items
            cgh.parallel_for(1024, [=] (id<1> idx) {
                // Initialize each buffer element with its own rank number
                writeResult[idx] = idx;
            }); // End of the kernel function
          
        }); // End of our commands for this queue
    } // End of scope, so we wait for work producing resultBuf to complete

    // Print result
    for(int i {0}; i < 1024; i++)
        std::cout << "data[" << i << "] = " << data[i] << std::endl;

    return 0; 
}