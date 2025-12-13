module;
#include <Metal/Metal.hpp>
module lune.metal;

namespace lune::metal
{
	void MetalBufferImpl::setData(const void* data, size_t size, const size_t offset)
	{
		if (!data || size == 0)
			return;

		// Clamp to avoid overrunning the buffer
		if (offset >= m_size)
			return;
		if (offset + size > m_size)
			size = m_size - offset;

		void* dst = static_cast<uint8_t*>(m_buffer->contents()) + offset;
		std::memcpy(dst, data, size);

		// Tell Metal the CPU modified the range
		m_buffer->didModifyRange(NS::Range{offset, size});
	}
} // namespace lune::metal
