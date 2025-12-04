#include <metal_stdlib>
using namespace metal;

struct VertexData
{
    float4 position;
    float2 uv;
};

struct Uniforms
{
    float time;
};

inline float4x4 makePerspective(float fovY_degrees, float aspect, float nearZ, float farZ)
{
    float fovY = fovY_degrees * (M_PI_F / 180.0f);
    float f = 1.0 / tan(fovY * 0.5);
    float A = farZ / (nearZ - farZ);
    float B = (farZ * nearZ) / (nearZ - farZ);

    return float4x4(
        float4(f / aspect, 0,  0,  0),
        float4(0,          f,  0,  0),
        float4(0,          0,  A, -1),
        float4(0,          0,  B,  0)
    );
}

inline float4x4 makeRotationY(float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    return float4x4(
        float4( c, 0, s, 0),
        float4( 0, 1, 0, 0),
        float4(-s, 0, c, 0),
        float4( 0, 0, 0, 1)
    );
}

struct VSOut
{
    float4 position [[position]];
    float2 uv;
};

vertex VSOut vertexMain(
    const device VertexData* cubeData [[buffer(0)]],
    constant Uniforms& u [[buffer(1)]],
    uint vid [[vertex_id]]
)
{
    VertexData v = cubeData[vid];
    float4 pos = v.position;

    float aspect = 1280.0 / 720.0;
    float4x4 proj = makePerspective(65.0, aspect, 0.1, 100.0);

    float4x4 view = float4x4(
        float4(1,0,0,0),
        float4(0,1,0,0),
        float4(0,0,1,0),
        float4(0,0,-3,1)
    );

    float4x4 rot = makeRotationY(u.time * 0.8);

    float4 worldPos = rot * pos;
    float4 clip = proj * (view * worldPos);

    VSOut out;
    out.position = clip;
    out.uv = v.uv;
    return out;
}

fragment float4 fragmentMain(VSOut in [[stage_in]])
{
    return float4(in.uv.x, in.uv.y, 1.0 - in.uv.x, 1.0);
}
