module;
export module lune:graphics_types;

namespace lune
{
	export enum BufferUsage
	{
		Managed,
		Memoryless,
		Private,
		Shared,
	};


	export enum PixelFormat
	{
		Undefined = 0,

		R8_UNorm,
		RG8_UNorm,
		RGBA8_UNorm,
		RGBA8_sRGB,

		BGRA8_UNorm,
		BGRA8_sRGB,

		R16_Float,
		RG16_Float,
		RGBA16_Float,

		R32_Float,
		RG32_Float,
		RGBA32_Float,
	};


	export enum PrimitiveType
	{
		Point,
		Line,
		LineStrip,
		Triangle,
		TriangleStrip,
	};
}
