StructuredBuffer<float3> vertexPositions;

struct VertexOutput
{
    float4 position : SV_Position;
};

struct FragmentOutput
{
    float4 color : SV_Target;
};

[shader("vertex")]
VertexOutput vertexMain(uint vertexID : SV_VertexID)
{
    return { float4(vertexPositions[vertexID], 1.0f)};
}

[shader("fragment")]
FragmentOutput fragmentMain(VertexOutput in)
{
    return { float4(182.0/255.0, 240.0/255.0, 228.0/255.0, 1.0) };
}
