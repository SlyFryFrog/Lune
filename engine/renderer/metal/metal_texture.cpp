module;
#include <iostream>
#include <Metal/Metal.hpp>
#include <stb_image.h>
module lune;

namespace lune::metal
{
	Texture::Texture(const TextureContextCreateInfo& createInfo) :
		m_info(createInfo)
	{
		if (!m_info.device)
			m_info.device = MetalContext::instance().device();

		create();
	}

	void Texture::load(const std::string& file, const int desiredChannelCount)
	{
		stbi_set_flip_vertically_on_load(true);
		int width, height, channelCount;
		unsigned char* image = stbi_load(file.c_str(), &width, &height, &channelCount,
		                                 desiredChannelCount);

		if (!image)
		{
			std::cerr << "Failed to load " << file << std::endl;
			return;
		}

		MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
		textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
		textureDescriptor->setWidth(width);
		textureDescriptor->setHeight(height);

		m_texture = NS::TransferPtr(m_info.device->newTexture(textureDescriptor));

		const MTL::Region region = MTL::Region(0, 0, 0, m_info.width, m_info.height, 1);
		const NS::UInteger bytesPerRow = 4 * width;

		m_texture->replaceRegion(region, 0, image, bytesPerRow);

		textureDescriptor->release();
		stbi_image_free(image);
	}

	void Texture::create()
	{
		const auto pixelFmt = toMetal(m_info.pixelFormat);
		MTL::TextureDescriptor* desc = MTL::TextureDescriptor::texture2DDescriptor(
			pixelFmt,
			m_info.width,
			m_info.height,
			m_info.mipmapped
			);
		desc->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite);

		m_texture = NS::TransferPtr(m_info.device->newTexture(desc));
	}
}
