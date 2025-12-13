module;
#include <memory>
export module lune.gfx:buffer;

import :types;

namespace lune::gfx
{
	/**
	 * @brief Platform-specific buffer implementation interface.
	 *
	 * @note Implemented by all supported backends.
	 */
	export struct IBufferImpl
	{
		virtual ~IBufferImpl() = default;

		virtual void setData(const void* data, size_t size, size_t offset) = 0;

		[[nodiscard]] virtual size_t size() const = 0;
		[[nodiscard]] virtual void* data() const = 0;
	};


	export class Buffer
	{
		std::unique_ptr<IBufferImpl> m_impl;

	public:
		explicit Buffer(std::unique_ptr<IBufferImpl> impl) noexcept : m_impl(std::move(impl))
		{
		}

		~Buffer() = default;

		/**
		 * @brief Gets the platform-specific implementation of a buffer.
		 *
		 * @note For internal use by backend code.
		 *
		 * @return Raw pointer to the platform-specific implementation.
		 */
		[[nodiscard]] IBufferImpl* getImpl() const
		{
			return m_impl.get();
		}

		void setData(const void* data, const size_t size, const size_t offset = 0) const
		{
			m_impl->setData(data, size, offset);
		}

		[[nodiscard]] size_t size() const noexcept
		{
			return m_impl->size();
		}

		[[nodiscard]] void* data() const noexcept
		{
			return m_impl->data();
		}
	};
} // namespace lune::gfx
