module;
#include <memory>
export module lune:buffer;

import :graphics_types;

namespace lune
{
	struct IBufferImpl
	{
		virtual ~IBufferImpl() = default;

		virtual void update(const void* data, size_t size, size_t offset) = 0;

		[[nodiscard]] virtual size_t size() const = 0;
		[[nodiscard]] virtual void* data() const = 0;
	};

	export class Buffer
	{
		std::unique_ptr<IBufferImpl> m_impl;

		friend IBufferImpl* getImpl(const Buffer& buffer);

	public:
		explicit Buffer(std::unique_ptr<IBufferImpl> impl) noexcept :
			m_impl(std::move(impl))
		{
		}

		~Buffer() = default;

		void update(const void* data, const size_t size, const size_t offset = 0) const
		{
			m_impl->update(data, size, offset);
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


	IBufferImpl* getImpl(const Buffer& buffer)
	{
		return buffer.m_impl.get();
	}
}
