// Slang shader

struct VertexInput
{
    uint vertexID : SV_VertexID;
};

struct VertexOutput
{
    float4 position : SV_Position;
};

// Buffer of positions
Buffer<float3> vertexPositions : register(t0);

VertexOutput vertexShader(VertexInput input)
{
    VertexOutput output;
    float3 pos3 = vertexPositions[input.vertexID];
    output.position = float4(pos3.x, pos3.y, pos3.z, 1.0);
    return output;
}

float4 fragmentShader(VertexOutput input) : SV_Target0
{
    return float4(182.0/255.0, 240.0/255.0, 228.0/255.0, 1.0);
}
