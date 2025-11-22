module;
#include <Metal/Metal.hpp>
export module lune:metal_texture;

import :metal_datatype_utils;

namespace lune::metal
{
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

		[[nodiscard]] MTL::Texture* texture() const
		{
			return m_shaderTexture.get();
		}
	private:
		void create();
	};
}
