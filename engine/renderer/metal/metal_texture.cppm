module;
#include <Metal/Metal.hpp>
#include <stb_image.h>
export module lune:metal_texture;

import :metal_context;
import :metal_mappings;

namespace lune::metal
{
	export struct TextureContextCreateInfo
	{
		MTL::Device* device{};
		PixelFormat pixelFormat{BGRA8_sRGB};
		int width{1};
		int height{1};
		bool mipmapped = false;
	};


	export class Texture
	{
		TextureContextCreateInfo m_info;
		NS::SharedPtr<MTL::Texture> m_texture;

	public:
		explicit Texture(const TextureContextCreateInfo& createInfo);
		~Texture() = default;

		[[nodiscard]] MTL::Texture* texture() const
		{
			return m_texture.get();
		}

		void load(const std::string& file, int desiredChannelCount = STBI_rgb_alpha);

	private:
		void create();
	};
}
