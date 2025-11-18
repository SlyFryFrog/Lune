#include <metal_stdlib>
using namespace metal;

kernel void generate_random(device float* outBuffer [[ buffer(0) ]],
                            uint tid [[ thread_position_in_grid ]])
{
    uint seed = tid * 747796405u + 2891336453u;
    seed ^= (seed >> 16);
    seed *= 0x45d9f3b;
    seed ^= (seed >> 16);
    float rnd = (float)(seed & 0xFFFFFF) / 16777216.0; // [0,1)
    outBuffer[tid] = rnd;
}
