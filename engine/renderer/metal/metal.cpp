module;
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <iostream>
module lune;

namespace lune::metal
{
	MetalContext::MetalContext()
	{
		createDevice();
	}

	MetalContext& MetalContext::instance()
	{
		static MetalContext s_instance;
		return s_instance;
	}

	void MetalContext::createDevice()
	{
		m_device = NS::TransferPtr(MTL::CreateSystemDefaultDevice());
		if (!m_device)
		{
			throw std::runtime_error("Failed to create Metal device");
		}

		m_commandQueue = NS::TransferPtr(m_device->newCommandQueue());
		if (!m_commandQueue)
		{
			throw std::runtime_error("Failed to create Metal command queue");
		}
	}

	CA::MetalLayer* MetalContext::createMetalLayer(const double width,
	                                               const double height)
	{
		m_metalLayer = NS::TransferPtr(CA::MetalLayer::layer());
		m_metalLayer->setDevice(m_device.get());
		m_metalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
		m_metalLayer->setFramebufferOnly(false);
		m_metalLayer->setDrawableSize(CGSize{width, height});
		return m_metalLayer.get();
	}

	void MetalContext::render()
	{
		if (!m_metalLayer || !m_commandQueue)
			return;

		// Get the next drawable
		const auto drawable = m_metalLayer->nextDrawable();
		if (!drawable)
			return;

		// Create a render pass descriptor
		const auto renderPassDescriptor = MTL::RenderPassDescriptor::renderPassDescriptor();
		const auto colorAttachment = renderPassDescriptor->colorAttachments()->object(0);
		colorAttachment->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0));
		colorAttachment->setLoadAction(MTL::LoadActionClear);
		colorAttachment->setStoreAction(MTL::StoreActionStore);
		colorAttachment->setTexture(drawable->texture());

		// Create a command buffer
		const auto commandBuffer = m_commandQueue->commandBuffer();
		const auto renderEncoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);
		setCurrentEncoder(renderEncoder);

		// Render all layers
		for (const auto& layer : m_layers)
			layer->render();

		renderEncoder->endEncoding();
		commandBuffer->presentDrawable(drawable);
		commandBuffer->commit();

		setCurrentEncoder(nullptr);
	}
}
