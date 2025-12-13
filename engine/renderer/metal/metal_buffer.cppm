module;
#include <Metal/Metal.hpp>
export module lune.metal:buffer;

import lune.gfx;

namespace lune::metal
{
	class MetalBufferImpl final : public gfx::IBufferImpl
	{
		NS::SharedPtr<MTL::Buffer> m_buffer{};
		size_t m_size{};

	public:
		MetalBufferImpl(MTL::Device* device, const size_t size) : m_size(size)
		{
			// Todo Allow setting the option externally through universal api
			m_buffer = NS::TransferPtr(
					device->newBuffer(static_cast<NS::Integer>(size), MTL::StorageModeShared));
		}

		void setData(const void* data, size_t size, size_t offset) override;

		[[nodiscard]] size_t size() const override
		{
			return m_size;
		}

		[[nodiscard]] void* data() const override
		{
			return m_buffer->contents();
		}

		[[nodiscard]] MTL::Buffer* buffer() const noexcept
		{
			return m_buffer.get();
		}
	};

	MetalBufferImpl* toMetalImpl(const gfx::Buffer& buffer)
	{
		const auto impl{getImpl(buffer)};

		// Optimize for speed in release builds
#ifndef NDEBUG
		const auto metalImpl{dynamic_cast<MetalBufferImpl*>(impl)};
		if (!metalImpl)
			throw std::runtime_error("Buffer is not a Metal buffer!");
		return metalImpl;
#else
		return static_cast<MetalBufferImpl*>(impl);
#endif
	}
} // namespace lune::metal
