module;
#include <Metal/Metal.hpp>
#include <iostream>
module lune.metal;

namespace lune::metal
{
	MetalContextImpl::MetalContextImpl()
	{
		createDefaultDevice();
		createCommandQueue();
	}

	MetalContextImpl& MetalContextImpl::instance()
	{
		static MetalContextImpl s_instance;
		return s_instance;
	}

	void MetalContextImpl::createDefaultDevice()
	{
		m_device = NS::TransferPtr(MTL::CreateSystemDefaultDevice());
		if (!m_device)
		{
			throw std::runtime_error("Failed to create Metal device");
		}
	}

	void MetalContextImpl::createCommandQueue()
	{
		m_commandQueue = NS::TransferPtr(m_device->newCommandQueue());
	}
} // namespace lune::metal
