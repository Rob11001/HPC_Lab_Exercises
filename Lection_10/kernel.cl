#define BLOCKSIZE 128
/**
    Naive reduction using local memory and tree based reduction
 */
__kernel void reduce_v1(__global float* out, __global float* in, unsigned int N, __local float* sdata) {
    unsigned int tid = get_local_id(0);
    unsigned int i = get_global_id(0);
    unsigned int localSize = get_local_size(0);

    // Load data from global memory to local memory
    sdata[tid] = (i < N) ? in[i] : 0;
    barrier(CLK_LOCAL_MEM_FENCE);   // Needed to be sure that every thread has loaded its part

    // Tree based redution
    for(unsigned int s = 1; s < localSize; s <<= 1) {
        if((tid % (2 * s)) == 0)            // Problem: this is highly divergent
            sdata[tid] += sdata[tid + s];

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    // Every work-group writes the result of its reduction
    if(tid == 0) 
        out[get_group_id(0)] = sdata[0];
}

/**
    "Sequential" reduction to avoid branch divergence
 */
__kernel void reduce_v2(__global float* out, __global float* in, unsigned int N, __local float* sdata) {
    unsigned int tid = get_local_id(0);
    unsigned int i = get_global_id(0);
    unsigned int localSize = get_local_size(0);

    // Load data from global memory to local memory
    sdata[tid] = (i < N) ? in[i] : 0;
    barrier(CLK_LOCAL_MEM_FENCE);   // Needed to be sure that every thread has loaded its part

    // Tree based redution
    for(unsigned int s = localSize / 2; s > 0; s >>= 1) {                           // Problem only half of the threads work
        if(tid < s)            // This reduce the divergence
            sdata[tid] += sdata[tid + s];

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    // Every work-group writes the result of its reduction
    if(tid == 0) 
        out[get_group_id(0)] = sdata[0];
}

/**
    Reduction with double load at the beginning
 */
__kernel void reduce_v3(__global float* out, __global float* in, unsigned int N, __local float* sdata) {
    unsigned int tid = get_local_id(0);
    unsigned int i = get_group_id(0) * (get_local_size(0) * 2) + tid;
    unsigned int localSize = get_local_size(0);

    // Load data from global memory to local memory
    // Every thread loads two elements
    sdata[tid] = (i < N) ? in[i] : 0;
    if(i + localSize < N) 
        sdata[tid] += in[i + localSize];

    barrier(CLK_LOCAL_MEM_FENCE);   // Needed to be sure that every thread has loaded its part

    // Tree based redution
    for(unsigned int s = localSize / 2; s > 0; s >>= 1) {                           
        if(tid < s)            // This reduce the divergence
            sdata[tid] += sdata[tid + s];

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    // Every work-group writes the result of its reduction
    if(tid == 0) 
        out[get_group_id(0)] = sdata[0];
}

/**
    Reduction with loop unrolling
 */
__kernel void reduce_v4(__global float* out, __global float* in, unsigned int N, __local volatile float* sdata) { // We need volatile here otherwise the threads can't see the value updated by others
    unsigned int tid = get_local_id(0);
    unsigned int i = get_group_id(0) * (get_local_size(0) * 2) + tid;
    unsigned int localSize = get_local_size(0);

    // Load data from global memory to local memory
    // Every thread loads two elements
    sdata[tid] = (i < N) ? in[i] : 0;
    if(i + localSize < N) 
        sdata[tid] += in[i + localSize];

    barrier(CLK_LOCAL_MEM_FENCE);   // Needed to be sure that every thread has loaded its part

    // Tree based redution
    for(unsigned int s = localSize / 2; s > 32; s >>= 1) {                           
        if(tid < s)            // This reduce the divergence
            sdata[tid] += sdata[tid + s];

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    // Loop unrolling of the last six iterations (Note: no need to barrier)
    if(tid < 32) {
        if(BLOCKSIZE >= 64) { sdata[tid] += sdata[tid + 32]; }
        if(BLOCKSIZE >= 32) { sdata[tid] += sdata[tid + 16]; }
        if(BLOCKSIZE >= 16) { sdata[tid] += sdata[tid + 8]; }
        if(BLOCKSIZE >= 8)  { sdata[tid] += sdata[tid + 4]; }
        if(BLOCKSIZE >= 4)  { sdata[tid] += sdata[tid + 2]; }
        if(BLOCKSIZE >= 2)  { sdata[tid] += sdata[tid + 1]; }
    }

    // Every work-group writes the result of its reduction
    if(tid == 0) 
        out[get_group_id(0)] = sdata[0];
}

/**
    Reduction with complete loop unrolling
 */
__kernel void reduce_v5(__global float* out, __global float* in, unsigned int N, __local volatile float* sdata) { // We need volatile here otherwise the threads can't see the value updated by others
    unsigned int tid = get_local_id(0);
    unsigned int i = get_group_id(0) * (get_local_size(0) * 2) + tid;
    unsigned int localSize = get_local_size(0);

    // Load data from global memory to local memory
    // Every thread loads two elements
    sdata[tid] = (i < N) ? in[i] : 0;
    if(i + localSize < N) 
        sdata[tid] += in[i + localSize];

    barrier(CLK_LOCAL_MEM_FENCE);   // Needed to be sure that every thread has loaded its part

    // Complete loop unrolling
    if(BLOCKSIZE >= 512) { if(tid < 256) { sdata[tid] += sdata[tid + 256]; } barrier(CLK_LOCAL_MEM_FENCE); }
    
    if(BLOCKSIZE >= 256) { if(tid < 128) { sdata[tid] += sdata[tid + 128]; } barrier(CLK_LOCAL_MEM_FENCE); }

    if(BLOCKSIZE >= 128) { if(tid < 64) { sdata[tid] += sdata[tid + 64]; } barrier(CLK_LOCAL_MEM_FENCE); }

    // Loop unrolling of the last six iterations (Note: no need to barrier)
    if(tid < 32) {
        if(BLOCKSIZE >= 64) { sdata[tid] += sdata[tid + 32]; }
        if(BLOCKSIZE >= 32) { sdata[tid] += sdata[tid + 16]; }
        if(BLOCKSIZE >= 16) { sdata[tid] += sdata[tid + 8]; }
        if(BLOCKSIZE >= 8)  { sdata[tid] += sdata[tid + 4]; }
        if(BLOCKSIZE >= 4)  { sdata[tid] += sdata[tid + 2]; }
        if(BLOCKSIZE >= 2)  { sdata[tid] += sdata[tid + 1]; }
    }

    // Every work-group writes the result of its reduction
    if(tid == 0) 
        out[get_group_id(0)] = sdata[0];
}