#include <metal_stdlib>
using namespace metal;

// Vertex output structure
struct VertexOut {
    float4 position [[position]];
    float3 color;
};

vertex VertexOut vertexMain(uint vertexID [[vertex_id]],
                            constant float3* vertexPositions,
                            constant float3* vertexColors)
{
    VertexOut out;
    out.position = float4(vertexPositions[vertexID], 1.0);
    out.color = vertexColors[vertexID]; // pass RGB color
    return out;
}

fragment float4 fragmentMain(VertexOut in [[stage_in]])
{
    return float4(in.color, 1.0);
}
