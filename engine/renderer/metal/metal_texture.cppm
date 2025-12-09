module;
#include <Metal/Metal.hpp>
export module lune:metal_texture;

import :metal_mappings;
import :texture;

namespace lune::metal
{
	class MetalTextureImpl final : public gfx::ITextureImpl
	{
		NS::SharedPtr<MTL::Texture> m_texture;

	public:
		explicit MetalTextureImpl(MTL::Device* device,
								  const gfx::TextureContextCreateInfo& createInfo);
		~MetalTextureImpl() override = default;

		void load(const std::string& file, int desiredChannelCount) override;

		[[nodiscard]] MTL::Texture* texture() const
		{
			return m_texture.get();
		}

	private:
		void create(MTL::Device* device);
	};


	constexpr MTL::Texture* toMetal(const gfx::Texture& texture)
	{
		auto* impl = gfx::getImpl(texture);

		// Optimize for speed in release builds
#ifndef NDEBUG
		const auto* metalImpl = dynamic_cast<MetalTextureImpl*>(impl);
		if (!metalImpl)
			throw std::runtime_error("Texture is not a Metal texture!");
		return metalImpl->texture();
#else
		return static_cast<MetalTextureImpl*>(impl)->texture();
#endif
	}
} // namespace lune::metal
