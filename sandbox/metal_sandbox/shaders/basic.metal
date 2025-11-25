#include <metal_stdlib>
using namespace metal;

vertex float4 vertexMain(uint vertexID [[vertex_id]],
                         constant float3* vertexPositions)
{
    return float4(vertexPositions[vertexID], 1.0);
}

fragment float4 fragmentMain(const constant float4& uColor [[buffer(0)]])
{
    return uColor;
}
