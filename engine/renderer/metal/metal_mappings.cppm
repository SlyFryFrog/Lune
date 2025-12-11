module;
#include <Metal/Metal.hpp>
export module lune.metal:metal_mappings;

import lune.gfx;

namespace lune::metal
{
	constexpr MTL::ResourceOptions toMetal(const gfx::BufferUsage usage) noexcept
	{
		using namespace lune::gfx;

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


	constexpr MTL::PixelFormat toMetal(const gfx::PixelFormat fmt) noexcept
	{
		using PF = gfx::PixelFormat;

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


	constexpr MTL::PrimitiveType toMetal(const gfx::PrimitiveType type) noexcept
	{
		using namespace lune::gfx;

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


	constexpr MTL::TriangleFillMode toMetal(const gfx::FillMode mode) noexcept
	{
		using namespace lune::gfx;

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

	constexpr MTL::CullMode toMetal(const gfx::CullMode mode) noexcept
	{
		using namespace lune::gfx;

		switch (mode)
		{
		case CullMode::Front:
			return MTL::CullModeFront;
		case CullMode::Back:
			return MTL::CullModeBack;
		default:
			return MTL::CullModeNone;
		}
	}

	constexpr MTL::Winding toMetal(const gfx::Winding mode) noexcept
	{
		using namespace lune::gfx;

		switch (mode)
		{
		case Winding::Clockwise:
			return MTL::WindingClockwise;
		case Winding::CounterClockwise:
		default:
			return MTL::WindingCounterClockwise;
		}
	}
} // namespace lune::metal
