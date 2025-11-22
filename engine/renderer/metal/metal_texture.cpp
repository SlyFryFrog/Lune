module;
#include <Metal/Metal.hpp>
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

	void Texture::create()
	{
		const auto pixelFmt = toMetalPixelFormat(m_info.pixelFormat);
		MTL::TextureDescriptor* desc = MTL::TextureDescriptor::texture2DDescriptor(
			pixelFmt,
			m_info.width,
			m_info.height,
			m_info.mipmapped
			);
		desc->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite);

		m_shaderTexture = NS::TransferPtr(m_info.device->newTexture(desc));
	}
}
