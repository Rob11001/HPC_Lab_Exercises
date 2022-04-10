#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <chrono>
#include <CL/opencl.h>
#include <CL/cl.h>

//#define DEBUG
#define BLOCK_DIM 8
 
char* load_kernel(const char * kernel_filename);

int main(int argc, char* argv[]) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    // Matrix dimensions
    unsigned int n, m;
 
    // Host input matrix
    float *h_a;
    // Host output matrix
    float *h_c;
 
    // Device input buffers
    cl_mem d_a;
    // Device output buffer
    cl_mem d_c;
 
    cl_platform_id cpPlatform;        // OpenCL platform
    cl_device_id device_id;           // device ID
    cl_context context;               // context
    cl_command_queue queue;           // command queue
    cl_program program;               // program
    cl_kernel kernel;                 // kernel
    
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <N> <M>\n", argv[0]);

        return EXIT_FAILURE;
    }
    n = atoi(argv[1]);
    m = atoi(argv[2]);

    // Size, in bytes, of each matrix
    size_t bytes = n * m * sizeof(float);
 
    // Allocate memory for each vector on host
    h_a = (float *) malloc(bytes);
    h_c = (float *) malloc(bytes);
 
    // Initialize vectors on host
    int i;
    for(i = 0; i < n * m; i++) {
        h_a[i] = i; 
    }

    // Load kernel source
    const char *kernel_src = load_kernel("naive_kernel.cl");
    if(!kernel_src) {
        fprintf(stderr, "Error: can't load source");
        return EXIT_FAILURE;
    }

    size_t globalSize[2], localSize[2];
    cl_int err;
 
    // Number of work items in each local work group
    localSize[0] = BLOCK_DIM;
    localSize[1] = BLOCK_DIM;
 
    // Number of total work items - localSize must be devisor
    globalSize[0] = ceil(n / (float) localSize[0]) * localSize[0];
    globalSize[1] = ceil(m / (float) localSize[1]) * localSize[1];
 
    // Bind to platform
    err = clGetPlatformIDs(1, &cpPlatform, NULL);
 
    // Get ID for the device
    err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

    if(err) {
        fprintf(stderr, "Error: %d\n", err);
        return EXIT_FAILURE;
    }
 
    // Create a context  
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
 
    // Create a command queue 
    queue = clCreateCommandQueue(context, device_id, 0, &err);

    // Create the compute program from the source buffer
    program = clCreateProgramWithSource(context, 1,
                            (const char **) & kernel_src, NULL, &err);
 
    // Build the program executable 
    clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
 
    // Create the compute kernel in the program we wish to run
    kernel = clCreateKernel(program, "transpose_naive", &err);
    
    // Begin time
    auto begin = high_resolution_clock::now();

    // Create the input and output arrays in device memory for our calculation
    d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
    d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, NULL);
 
    // Write our data set into the input array in device memory
    err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0,
                                   bytes, h_a, 0, NULL, NULL);

    // Set the arguments to our compute kernel
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_c);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_a);
    err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &n);
    err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &m);

    // Execute the kernel over the entire range of the data set  
    auto startKernel = high_resolution_clock::now();
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, localSize,
                                                              0, NULL, NULL);
    // Wait for the command queue to get serviced before reading back results
    clFinish(queue);
    auto endKernel = high_resolution_clock::now();
    
    // Read the results from the device
    clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0,
                                bytes, h_c, 0, NULL, NULL );
    
    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();
    ms_double = endKernel - startKernel;
    float kernelTime = ms_double.count();

    #ifdef DEBUG
        printf("Time: %f\nKernel: %f\n", time, kernelTime);
        if(n < 16 && m < 16) {
            // Check result
            printf("A:\n");
            for(i = 0; i < n * m; i++) {
                if(i % n == 0 && i != 0)
                    printf("\n");
                printf("A[%d][%d] = %.2f ", i / n, i % n, h_a[i]);
            }
        
            printf("\n\nC:\n");
            for(i = 0; i < n * m; i++) {
                if(i % n == 0 && i != 0)
                    printf("\n");
                printf("C[%d][%d] = %.2f ", i / n, i % n, h_c[i]);
            }
        }
    #endif

    #ifndef DEBUG
        printf("%f, %f", time, kernelTime);
    #endif

    // release OpenCL resources
    clReleaseMemObject(d_a);
    clReleaseMemObject(d_c);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    
    //release host memory
    free(h_a);
    free(h_c);
 
    return 0;
}


char * load_kernel(const char * kernel_filename) {
    FILE *fp;
    char *source_str;
    size_t source_size, program_size;

    fp = fopen(kernel_filename, "rb");
    if (!fp) {
        printf("Failed to load kernel\n");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    program_size = ftell(fp);
    rewind(fp);
    source_str = (char*) malloc(program_size + 1);
    source_str[program_size] = '\0';
    fread(source_str, sizeof(char), program_size, fp);
    fclose(fp);

    return source_str;
}