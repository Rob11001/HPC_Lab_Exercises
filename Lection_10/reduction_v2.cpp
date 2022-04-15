#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <chrono>
#include <CL/opencl.h>
#include <CL/cl.h>

//#define DEBUG
#define BLOCK_DIM 128
#define KERNEL_SRC_FILENAME "kernel.cl"
#define KERNEL_FUNCTION_NAME "reduce_v2"
 
char* load_kernel(const char * kernel_filename);

int main(int argc, char* argv[]) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    // Array dimension
    unsigned int N;
 
    // Host input array
    float *h_a;
    // Host output array
    float *h_c;
 
    // Device input buffer
    cl_mem d_a;
    // Device output buffer
    cl_mem d_c;
 
    cl_platform_id cpPlatform;        // OpenCL platform
    cl_device_id device_id;           // device ID
    cl_context context;               // context
    cl_command_queue queue;           // command queue
    cl_program program;               // program
    cl_kernel kernel;                 // kernel
    
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);

        return EXIT_FAILURE;
    }

    N = atoi(argv[1]);
    
    // Size, in bytes
    size_t bytes = N * sizeof(float);
    
    // Group_size
    size_t globalSize, localSize;
    cl_int err;
 
    // Number of work items in each local work group
    localSize = BLOCK_DIM;
 
    // Number of total work items - localSize must be devisor
    globalSize = ceil(N / (float) localSize) * localSize;
 
    // Allocate memory for each vector on host
    h_a = (float *) malloc(bytes);
    h_c = (float *) malloc((globalSize / BLOCK_DIM) * sizeof(float));
 
    // Initialize vectors on host
    int i;
    for(i = 0; i < N; i++) {
        h_a[i] = rand() % 10; 
    }

    // Load kernel source
    const char *kernel_src = load_kernel(KERNEL_SRC_FILENAME);
    if(!kernel_src) {
        fprintf(stderr, "Error: can't load source");
        return EXIT_FAILURE;
    }


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
    queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);

    // Create the compute program from the source buffer
    program = clCreateProgramWithSource(context, 1,
                            (const char **) &kernel_src, NULL, &err);
 
    // Build the program executable 
    clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
 
    // Create the compute kernel in the program we wish to run
    kernel = clCreateKernel(program, KERNEL_FUNCTION_NAME, &err);
    
    // Begin time
    auto begin = high_resolution_clock::now();

    // Create the input and output arrays in device memory for our calculation
    d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
    d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, (globalSize / BLOCK_DIM) * sizeof(float), NULL, NULL);
 
    // Write our data set into the input array in device memory
    err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0,
                                   bytes, h_a, 0, NULL, NULL);

    /* Two stage reduction */
    
    // Set the arguments to our compute kernel
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_c);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_a);
    err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &N);
    err |= clSetKernelArg(kernel, 3, sizeof(float) * BLOCK_DIM, NULL);

    // Execute the kernel over the entire range of the data set  
    
    // First stage
    cl_event event;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize,
                                                              0, NULL, &event);
    // Wait for the command queue to get serviced before reading back results
    clFinish(queue);
    
    // Read the results from the device
    clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0,
                                (globalSize / BLOCK_DIM) * sizeof(float), h_c, 0, NULL, NULL );

    // Second stage
    // Sequential reduction
    float acc = h_c[0];
    for (int i = 1; i < globalSize / BLOCK_DIM; i++) {
        acc += h_c[i];
    }

    // End time
    auto end = high_resolution_clock::now();
    duration<double, std::milli> ms_double = end - begin;
    float time = ms_double.count();

    cl_ulong time_start;
    cl_ulong time_end;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
    double nanoSeconds = time_end-time_start;

    #ifdef DEBUG
        printf("Time: %f\nKernel: %f\n", time, kernelTime);
        if(N < 32) {
            // Check result
            printf("A:\n");
            for(i = 0; i < N; i++) {
                printf("A[%d] = %.2f ", i, h_a[i]);
            }
        }
        printf("\n\nReduced value: %.2f\n", acc);
    #endif

    #ifndef DEBUG
        printf("%f, %f", time, nanoSeconds / 1000000.0);
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
    free((void *) kernel_src);
 
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