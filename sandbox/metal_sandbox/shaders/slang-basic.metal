#include <metal_stdlib>
#include <metal_math>
#include <metal_texture>
using namespace metal;

#line 4 "basic.slang"
struct VertexOutput_0
{
    float4 position_0;
};


#line 4
VertexOutput_0 VertexOutput_x24init_0(const float4 thread* position_1)
{

#line 4
    thread VertexOutput_0 _S1;

    (&_S1)->position_0 = *position_1;

#line 4
    return _S1;
}



struct FragmentOutput_0
{
    float4 color_0 [[color(0)]];
};


#line 9
FragmentOutput_0 FragmentOutput_x24init_0(const float4 thread* color_1)
{

#line 9
    thread FragmentOutput_0 _S2;

    (&_S2)->color_0 = *color_1;

#line 9
    return _S2;
}


#line 21
[[fragment]] FragmentOutput_0 fragmentMain(float4 position_2 [[position]], float3 device* vertexPositions_0 [[buffer(0)]])
{

#line 21
    float4 _S3 = float4(0.7137255072593689, 0.94117647409439087, 0.89411765336990356, 1.0);

#line 21
    FragmentOutput_0 _S4 = FragmentOutput_x24init_0(&_S3);

    return _S4;
}


#line 23
struct vertexMain_Result_0
{
    float4 position_3 [[position]];
};


#line 4
[[vertex]] vertexMain_Result_0 vertexMain(uint vertexID_0 [[vertex_id]], float3 device* vertexPositions_1 [[buffer(0)]])
{

#line 4
    float4 _S5 = float4(*(vertexPositions_1+vertexID_0), 1.0);

#line 4
    VertexOutput_0 _S6 = VertexOutput_x24init_0(&_S5);

#line 4
    thread vertexMain_Result_0 _S7;

#line 4
    (&_S7)->position_3 = _S6.position_0;

#line 4
    return _S7;
}

