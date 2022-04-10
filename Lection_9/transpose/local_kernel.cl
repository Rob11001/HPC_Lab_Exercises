// Transpose the matrix using tiling in local memory
__kernel void transpose(__global float *odata, __global float* idata, __local float* block, int width, int height) {
    unsigned int xIndex = get_global_id(0);
    unsigned int yIndex = get_global_id(1);
    unsigned int xLocalIndex = get_local_id(0);
    unsigned int yLocalIndex = get_local_id(1);
    unsigned int tileSize = get_local_size(0) + 1; // Plus one to try to reduce bank conflicts
    
    // read the matrix tile into local memory
    if (xIndex < width && yIndex < height) {
        unsigned int index_in = xIndex + width * yIndex; // Coalesced access
        block[xLocalIndex + yLocalIndex * tileSize] = idata[index_in];
    }

    barrier(CLK_LOCAL_MEM_FENCE); // Wait until all threads in the work-group have loaded the tile in memory

    // write the transposed matrix tile to global memory
    if(xIndex < height && yIndex < width) {
        unsigned int index_out = xIndex + height * yIndex; // Coalesced access
        odata[index_out] = block[yLocalIndex + xLocalIndex * tileSize]; // Maybe here there are some bank conflicts
        //printf("Thread %d (%d-%d) - block %d - bank %d\n", xIndex + yIndex * width, xIndex, yIndex, yLocalIndex + xLocalIndex * tileSize, (yLocalIndex + xLocalIndex * tileSize) % 32);
    }
}

#define BLOCK_DIM 16

__kernel void transpose_from_github(__global float *odata, __global float *idata, __local float* block, int width, int height) {
	// read the matrix tile into shared memory
	unsigned int xIndex = get_global_id(0);
	unsigned int yIndex = get_global_id(1);

	if((xIndex < width) && (yIndex < height))
	{
		unsigned int index_in = yIndex * width + xIndex;
		block[get_local_id(1)*(BLOCK_DIM+1)+get_local_id(0)] = idata[index_in];
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	// write the transposed matrix tile to global memory
	xIndex = get_group_id(1) * BLOCK_DIM + get_local_id(0);
	yIndex = get_group_id(0) * BLOCK_DIM + get_local_id(1);
	if((xIndex < height) && (yIndex < width))
    {
		unsigned int index_out = yIndex * height + xIndex;
		odata[index_out] = block[get_local_id(0)*(BLOCK_DIM+1)+get_local_id(1)];
	}
}