module;
#include <Metal/Metal.hpp>
export module lune:metal_mappings;

import :graphics_types;

namespace lune::metal
{
	MTL::ResourceOptions toMetal(const BufferUsage usage)
	{
		switch (usage)
		{
		case BufferUsage::Managed:
			return MTL::StorageModeManaged;
		case BufferUsage::Memoryless:
			return MTL::StorageModeMemoryless;
		case BufferUsage::Private:
			return MTL::StorageModePrivate;
		case BufferUsage::Shared:
		default:
			return MTL::StorageModeShared;
		}
	}


	MTL::PixelFormat toMetal(const PixelFormat fmt)
	{
		using PF = PixelFormat;

		switch (fmt)
		{
		case PF::R8_UNorm:
			return MTL::PixelFormatR8Unorm;
		case PF::RG8_UNorm:
			return MTL::PixelFormatRG8Unorm;
		case PF::RGBA8_UNorm:
			return MTL::PixelFormatRGBA8Unorm;
		case PF::RGBA8_sRGB:
			return MTL::PixelFormatRGBA8Unorm_sRGB;

		case PF::BGRA8_UNorm:
			return MTL::PixelFormatBGRA8Unorm;
		case PF::BGRA8_sRGB:
			return MTL::PixelFormatBGRA8Unorm_sRGB;

		case PF::R16_Float:
			return MTL::PixelFormatR16Float;
		case PF::RG16_Float:
			return MTL::PixelFormatRG16Float;
		case PF::RGBA16_Float:
			return MTL::PixelFormatRGBA16Float;

		case PF::R32_Float:
			return MTL::PixelFormatR32Float;
		case PF::RG32_Float:
			return MTL::PixelFormatRG32Float;
		case PF::RGBA32_Float:
			return MTL::PixelFormatRGBA32Float;

		case PF::Depth16_UNorm:
			return MTL::PixelFormatDepth16Unorm;
		case PF::Depth32_Float:
			return MTL::PixelFormatDepth32Float;
		case PF::Depth24_UNorm_Stencil8:
			return MTL::PixelFormatDepth24Unorm_Stencil8;
		case PF::Depth32_Float_Stencil8:
			return MTL::PixelFormatDepth32Float_Stencil8;

		default:
			return MTL::PixelFormatInvalid;
		}
	}


	MTL::PrimitiveType toMetal(const PrimitiveType type)
	{
		switch (type)
		{
		case PrimitiveType::Point:
			return MTL::PrimitiveTypePoint;
		case PrimitiveType::Line:
			return MTL::PrimitiveTypeLine;
		case PrimitiveType::LineStrip:
			return MTL::PrimitiveTypeLineStrip;
		case PrimitiveType::Triangle:
			return MTL::PrimitiveTypeTriangle;
		case PrimitiveType::TriangleStrip:
			return MTL::PrimitiveTypeTriangleStrip;
		default:
			return MTL::PrimitiveTypeTriangle;
		}
	}


	inline MTL::TriangleFillMode toMetal(const FillMode mode)
	{
		switch (mode)
		{
		case FillMode::Fill:
			return MTL::TriangleFillModeFill;
		case FillMode::Wireframe:
			return MTL::TriangleFillModeLines;
		default:
			return MTL::TriangleFillModeFill;
		}
	}
}
