#include <metal_stdlib>
using namespace metal;

kernel void computeNextStateBuffer(
    device const uchar4* inBuff [[buffer(0)]],
    device uchar4* outBuff [[buffer(1)]],
    constant int& width [[buffer(2)]],
    constant int& height [[buffer(3)]],
    constant uchar4& cellColor [[buffer(4)]],
    constant uchar4& backgroundColor [[buffer(5)]],
    uint2 gid [[thread_position_in_grid]])
{
    if (gid.x >= width || gid.y >= height) return;  // Out of bounds

    uint index = gid.y * width + gid.x;

    uchar4 cell = inBuff[index];
    bool alive = any(cell != backgroundColor);    // If cell has color

    int aliveNeighbors = 0;

    // Iterate through all neighbor cells
    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            // Skip the middle (we are the middle cell)
            if (dx == 0 && dy == 0) continue;
            int nx = int(gid.x) + dx;
            int ny = int(gid.y) + dy;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height)
            {
                uchar4 n = inBuff[ny * width + nx];
                aliveNeighbors += any(n > backgroundColor) ? 1 : 0;
            }
        }
    }

    bool nextAlive =
        (alive && (aliveNeighbors == 2 || aliveNeighbors == 3)) ||
        (!alive && aliveNeighbors == 3);

    outBuff[index] = nextAlive
        ? (cellColor ? cellColor : uchar4(255, 0, 0, 255))
        : backgroundColor;
}
