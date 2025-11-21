module;
#include <Metal/Metal.hpp>
export module lune:metal_texture;

namespace lune::metal
{
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

		default:
			return MTL::PixelFormatInvalid;
		}
	}


	export struct TextureContextCreateInfo
	{
		MTL::Device* device{};
		PixelFormat pixelFormat{};
		int width{};
		int height{};
		bool mipmapped = false;
	};


	export class Texture
	{
		TextureContextCreateInfo m_info;
		NS::SharedPtr<MTL::Texture> m_shaderTexture;

	public:
		explicit Texture(const TextureContextCreateInfo& createInfo);
		~Texture() = default;

		void create();

		[[nodiscard]] MTL::Texture* texture() const;

	private:
	};
}
