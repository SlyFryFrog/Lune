#include <metal_stdlib>
using namespace metal;

kernel void computeNextStateBuffer(
    device uint4* dataBuffer [[buffer(0)]],
    uint2 gid [[thread_position_in_grid]])
{
    constexpr uint Width = 1024;    // TODO pass width and height as params
    constexpr uint Height = 728;

    if (gid.x >= Width || gid.y >= Height) return;

    uint index = gid.y * Width + gid.x;

    int aliveNeighbors = 0;

    // Loop over neighbors
    for (int dy = -1; dy <= 1; ++dy)
    for (int dx = -1; dx <= 1; ++dx)
    {
        if (dx == 0 && dy == 0) continue;
        int nx = int(gid.x) + dx;
        int ny = int(gid.y) + dy;
        if (nx >= 0 && nx < Width && ny >= 0 && ny < Height)
        {
            int neighborIndex = ny * Width + nx;
            aliveNeighbors += dataBuffer[neighborIndex].r > 0 ? 1 : 0;
        }
    }

    bool alive = dataBuffer[index].r > 0;

    // Game of Life rules
    bool nextAlive = (alive && (aliveNeighbors == 2 || aliveNeighbors == 3)) ||
                     (!alive && aliveNeighbors == 3);

    dataBuffer[index] = nextAlive ? uint4(255, 0, 0, 0) : uint4(0, 0, 0, 0);
}
