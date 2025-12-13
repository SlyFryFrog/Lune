module;
#include <Metal/Metal.hpp>
export module lune.metal:texture;

import lune.gfx;

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

		[[nodiscard]] MTL::Texture* texture() const noexcept
		{
			return m_texture.get();
		}

	private:
		void create(MTL::Device* device);
	};


	constexpr MetalTextureImpl* toMetalImpl(const gfx::Texture& texture)
	{
		auto* impl = gfx::getImpl(texture);

#ifndef NDEBUG
		const auto metalImpl = dynamic_cast<MetalTextureImpl*>(impl);
		if (!metalImpl)
			throw std::runtime_error("Texture is not a Metal texture!");
		return metalImpl;
#else
		return static_cast<MetalTextureImpl*>(impl);
#endif
	}
} // namespace lune::metal
