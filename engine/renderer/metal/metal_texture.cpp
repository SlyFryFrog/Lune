module;
#include <Metal/Metal.hpp>
#include <iostream>
#include <stb_image.h>
module lune;

namespace lune::metal
{
	MetalTextureImpl::MetalTextureImpl(MTL::Device* device,
									   const gfx::TextureContextCreateInfo& createInfo) :
		ITextureImpl(createInfo)
	{
		create(device);
	}

	void MetalTextureImpl::load(const std::string& file, const int desiredChannelCount)
	{
		stbi_set_flip_vertically_on_load(true);
		int channelCount;
		unsigned char* image{stbi_load(file.c_str(), &m_info.width, &m_info.height, &channelCount,
									   desiredChannelCount)};

		if (!image)
		{
			std::cerr << "Failed to load " << file << std::endl;
			return;
		}

		MTL::TextureDescriptor* textureDescriptor{MTL::TextureDescriptor::alloc()->init()};
		textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
		textureDescriptor->setWidth(m_info.width);
		textureDescriptor->setHeight(m_info.height);

		m_texture = NS::TransferPtr(m_texture->device()->newTexture(textureDescriptor));

		const MTL::Region region{0,
								 0,
								 0,
								 static_cast<NS::UInteger>(m_info.width),
								 static_cast<NS::UInteger>(m_info.height),
								 1};
		const NS::UInteger bytesPerRow{static_cast<NS::UInteger>(4 * m_info.width)};

		m_texture->replaceRegion(region, 0, image, bytesPerRow);
		textureDescriptor->release();
		stbi_image_free(image);
	}

	void MetalTextureImpl::create(MTL::Device* device)
	{
		const auto pixelFmt = toMetal(m_info.pixelFormat);
		MTL::TextureDescriptor* desc{MTL::TextureDescriptor::texture2DDescriptor(
				pixelFmt, m_info.width, m_info.height, m_info.mipmapped)};
		desc->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite);

		m_texture = NS::TransferPtr(device->newTexture(desc));
	}
} // namespace lune::metal
