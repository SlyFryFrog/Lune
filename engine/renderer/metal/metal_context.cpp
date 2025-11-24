module;
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <iostream>
module lune;

import :file;

namespace lune::metal
{
	MetalContext::MetalContext()
	{
		createDefaultDevice();
		createCommandQueue();
	}

	MetalContext& MetalContext::instance()
	{
		static MetalContext s_instance;
		return s_instance;
	}

	void MetalContext::create(const MetalContextCreateInfo& info)
	{
		m_createInfo = info;
	}

	void MetalContext::createDefaultDevice()
	{
		m_device = NS::TransferPtr(MTL::CreateSystemDefaultDevice());
		if (!m_device)
		{
			throw std::runtime_error("Failed to create Metal device");
		}
	}

	void MetalContext::createCommandQueue()
	{
		m_commandQueue = NS::TransferPtr(m_device->newCommandQueue());
	}

	void MetalContext::addMetalLayer(const NS::SharedPtr<CA::MetalLayer>& metalLayer)
	{
		m_metalLayers.push_back(metalLayer);
	}

	void MetalContext::removeMetalLayer(const NS::SharedPtr<CA::MetalLayer>& metalLayer)
	{
		std::erase_if(
			m_metalLayers,
			[&](const NS::SharedPtr<CA::MetalLayer>& ptr)
			{
				return ptr.get() == metalLayer.get();
			}
			);
	}
}
