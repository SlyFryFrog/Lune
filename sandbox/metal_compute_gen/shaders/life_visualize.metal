#include <metal_stdlib>
using namespace metal;

struct VSOut
{
    float4 position [[position]];
    float2 uv;
};

vertex VSOut vertexMain(uint vid [[vertex_id]],
                        const device float2* verts [[buffer(0)]],
                        constant float& zoom [[buffer(1)]])
{
    VSOut out;
    out.position = float4(verts[vid], 0.0, 1.0);
    float2 uv = (verts[vid] + 1.0) * 0.5;
    uv = uv * (zoom ? zoom : 0.2f);
    out.uv = uv;
    return out;
}

fragment float4 fragmentMain(VSOut in [[stage_in]],
                             texture2d<float, access::sample> tex [[texture(0)]],
                             sampler samp [[sampler(0)]])
{
    return tex.sample(samp, in.uv);
}
