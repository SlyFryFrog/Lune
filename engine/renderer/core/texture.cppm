module;
#include <memory>
#include <stb_image.h>
export module lune:texture;

import :graphics_types;

namespace lune
{
	export struct TextureContextCreateInfo
	{
		PixelFormat pixelFormat{BGRA8_sRGB};
		int width{1};
		int height{1};
		bool mipmapped = false;
	};

	struct ITextureImpl
	{
		TextureContextCreateInfo m_info;

		explicit ITextureImpl(const TextureContextCreateInfo& createInfo) :
			m_info(createInfo)
		{
		}

		virtual ~ITextureImpl() = default;

		virtual void load(const std::string& path, int desiredChannelCount) = 0;
	};

	export class Texture
	{
		std::unique_ptr<ITextureImpl> m_impl;

		friend ITextureImpl* getImpl(const Texture& texture);

	public:
		explicit Texture(std::unique_ptr<ITextureImpl> impl) noexcept :
			m_impl(std::move(impl))
		{
		}

		~Texture() = default;

		void load(const std::string& path, const int desiredChannelCount = STBI_rgb_alpha) const
		{
			m_impl->load(path, desiredChannelCount);
		}

		[[nodiscard]] int width() const
		{
			return m_impl->m_info.width;
		}

		[[nodiscard]] int height() const
		{
			return m_impl->m_info.height;
		}

		[[nodiscard]] bool mipmapped() const
		{
			return m_impl->m_info.mipmapped;
		}

		[[nodiscard]] PixelFormat pixelFormat() const
		{
			return m_impl->m_info.pixelFormat;
		}
	};


	ITextureImpl* getImpl(const Texture& texture)
	{
		return texture.m_impl.get();
	}
}
