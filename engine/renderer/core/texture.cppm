module;
#include <stb_image.h>
#include <string>
export module lune:texture;

import :graphics_types;

namespace lune::gfx
{
	/**
	 * @brief Configuration for texture creation.
	 */
	export struct TextureContextCreateInfo
	{
		PixelFormat pixelFormat{BGRA8_sRGB}; ///< Texture pixel format.
		int width{1};						 ///< Texture width in pixels.
		int height{1};						 ///< Texture height in pixels.
		bool mipmapped = false;				 ///< Whether to generate mipmaps.
	};

	/**
	 * @brief Platform-specific texture implementation interface.
	 *
	 * @note Not exposed in public API.
	 */
	struct ITextureImpl
	{
		TextureContextCreateInfo m_info; ///< Texture configuration.

		/**
		 * @brief Constructs a texture implementation.
		 * @param createInfo Texture configuration.
		 */
		explicit ITextureImpl(const TextureContextCreateInfo& createInfo) : m_info(createInfo)
		{
		}

		virtual ~ITextureImpl() = default;

		/**
		 * @brief Loads texture data from a file.
		 *
		 * @param path File path to load from.
		 * @param desiredChannelCount Number of channels to load (e.g., STBI_rgb_alpha).
		 */
		virtual void load(const std::string& path, int desiredChannelCount) = 0;
	};

	/**
	 * @brief Cross-platform texture wrapper.
	 *
	 * Manages a texture and its platform-specific implementation.
	 */
	export class Texture
	{
		std::unique_ptr<ITextureImpl> m_impl; ///< Platform-specific implementation.
		friend ITextureImpl* getImpl(const Texture& texture);

	public:
		/**
		 * @brief Constructs a texture with a platform-specific implementation.
		 *
		 * @param impl Platform-specific implementation.
		 */
		explicit Texture(std::unique_ptr<ITextureImpl> impl) noexcept : m_impl(std::move(impl))
		{
		}

		~Texture() = default;

		/**
		 * @brief Loads a texture from a file.
		 *
		 * @param path File path to load from.
		 * @param desiredChannelCount Number of channels to load (default: STBI_rgb_alpha).
		 */
		void load(const std::string& path, const int desiredChannelCount = STBI_rgb_alpha) const
		{
			m_impl->load(path, desiredChannelCount);
		}

		/**
		 * @brief Returns texture width in pixels.
		 */
		[[nodiscard]] int width() const
		{
			return m_impl->m_info.width;
		}

		/**
		 * @brief Returns texture height in pixels.
		 */
		[[nodiscard]] int height() const
		{
			return m_impl->m_info.height;
		}

		/**
		 * @brief Returns whether the texture uses mipmapping.
		 */
		[[nodiscard]] bool mipmapped() const
		{
			return m_impl->m_info.mipmapped;
		}

		/**
		 * @brief Returns the texture's pixel format.
		 */
		[[nodiscard]] PixelFormat pixelFormat() const
		{
			return m_impl->m_info.pixelFormat;
		}
	};

	/**
	 * @brief Gets the platform-specific implementation of a texture.
	 *
	 * @note For internal use by backend code.
	 *
	 * @param texture Texture to retrieve implementation from.
	 * @return Raw pointer to the platform-specific implementation.
	 */
	ITextureImpl* getImpl(const Texture& texture)
	{
		return texture.m_impl.get();
	}
} // namespace lune::gfx
