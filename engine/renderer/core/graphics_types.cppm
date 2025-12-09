module;
export module lune:graphics_types;

namespace lune::gfx
{
	export enum BufferUsage
	{
		Managed,
		Memoryless,
		Private,
		Shared,
	};


	export enum StorageUsage
	{
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

		Depth16_UNorm,
		Depth32_Float,
		Depth24_UNorm_Stencil8,
		Depth32_Float_Stencil8,
	};


	export enum PrimitiveType
	{
		Point,
		Line,
		LineStrip,
		Triangle,
		TriangleStrip,
	};


	export enum FillMode
	{
		Fill,
		Wireframe
	};

	export enum CullMode
	{
		Front,
		Back,
		None
	};

	export enum Winding
	{
		Clockwise,
		CounterClockwise
	};
} // namespace lune::gfx
