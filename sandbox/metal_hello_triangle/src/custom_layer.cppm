module;
#include <simd/vector.h>
export module custom_layer;

import lune;


struct TriangleData
{
	vector_float3 positions[3];
};


TriangleData triangle = {{
	{0.0f, 0.5f, 0.0f},   // Top vertex
	{-0.5f, -0.5f, 0.0f}, // Bottom-left vertex
	{0.5f, -0.5f, 0.0f}   // Bottom-right vertex
}};


export class CustomLayer
{
public:
	CustomLayer();
	~CustomLayer();
};