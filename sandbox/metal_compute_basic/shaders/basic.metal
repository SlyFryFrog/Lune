#include <metal_stdlib>
using namespace metal;

// Kernel 1: Add two arrays
kernel void add_arrays(
    device const float* inputA [[buffer(0)]],
    device const float* inputB [[buffer(1)]],
    device float* output [[buffer(2)]],
    uint id [[thread_position_in_grid]]
) {
    output[id] = inputA[id] + inputB[id];
}

// Kernel 2: Multiply two arrays
kernel void multiply_arrays(
    device const float* inputA [[buffer(0)]],
    device const float* inputB [[buffer(1)]],
    device float* output [[buffer(2)]],
    uint id [[thread_position_in_grid]]
) {
    output[id] = inputA[id] * inputB[id];
}
