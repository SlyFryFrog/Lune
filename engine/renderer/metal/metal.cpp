module;
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
module lune;

namespace lune::metal
{
	MetalContext::MetalContext()
	{
		// Create the Metal device
		m_device = MTL::CreateSystemDefaultDevice();
		if (!m_device)
			throw std::runtime_error("Failed to create Metal device");

		// Create the command queue
		m_commandQueue = m_device->newCommandQueue();
		if (!m_commandQueue)
			throw std::runtime_error("Failed to create Metal command queue");

		// Create the Metal layer
		m_metalLayer = CA::MetalLayer::layer();
		m_metalLayer->retain();
		if (!m_metalLayer)
			throw std::runtime_error("Failed to create CA::MetalLayer");

		// Configure the Metal layer
		m_metalLayer->setDevice(m_device);
		m_metalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
		m_metalLayer->setFramebufferOnly(false);
	}

	MetalContext::~MetalContext()
	{
		if (m_metalLayer)
		{
			m_metalLayer->release();
			m_metalLayer = nullptr;
		}
		if (m_commandQueue)
		{
			m_commandQueue->release();
			m_commandQueue = nullptr;
		}
		if (m_device)
		{
			m_device->release();
			m_device = nullptr;
		}
	}

	MetalContext& MetalContext::instance()
	{
		static MetalContext s_instance;
		return s_instance;
	}

	void MetalContext::render()
	{

	}
}
