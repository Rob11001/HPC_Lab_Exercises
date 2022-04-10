// This naive transpose kernel suffers from completely non-coalesced writes.
__kernel void transpose_naive(__global float *odata, __global float* idata, int width, int height)
{
    unsigned int xIndex = get_global_id(0);
    unsigned int yIndex = get_global_id(1);
    
    if (xIndex < width && yIndex < height) {
        unsigned int index_in = xIndex + width * yIndex;
        unsigned int index_out = yIndex + height * xIndex;
        odata[index_out] = idata[index_in]; 
    }
}