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

	void MetalContext::sendRenderCommands()
	{
		m_commandBuffer = NS::TransferPtr(m_commandQueue->commandBuffer());

		MTL::RenderPassDescriptor* renderPassDescriptor =
			MTL::RenderPassDescriptor::alloc()->init();
		MTL::RenderPassColorAttachmentDescriptor* colorAttachmentDescriptor =
			renderPassDescriptor->colorAttachments()->object(0);
		colorAttachmentDescriptor->setTexture(m_drawable->texture());
		colorAttachmentDescriptor->setLoadAction(m_createInfo.loadAction);
		colorAttachmentDescriptor->setClearColor(m_createInfo.clearColor);
		colorAttachmentDescriptor->setStoreAction(m_createInfo.storeAction);

		MTL::RenderCommandEncoder* renderCommandEncoder = m_commandBuffer->renderCommandEncoder(
			renderPassDescriptor);

		for (const auto& shader : m_graphicsShaders)
		{
			shader->encodeRenderCommand(renderCommandEncoder);
		}

		renderCommandEncoder->endEncoding();

		m_commandBuffer->presentDrawable(m_drawable.get());
		m_commandBuffer->commit();
		m_commandBuffer->waitUntilCompleted();

		renderPassDescriptor->release();
	}

	void MetalContext::sendComputeCommands()
	{
		// Create a new command buffer for compute
		m_commandBuffer = NS::TransferPtr(m_commandQueue->commandBuffer());

		// Encode compute commands for all compute shaders
		MTL::ComputeCommandEncoder* computeCommandEncoder = m_commandBuffer->computeCommandEncoder();
		for (const auto& shader : m_computeShaders)
		{
			shader->encodeComputeCommand(computeCommandEncoder);
		}
		computeCommandEncoder->endEncoding();

		// Commit and wait for completion
		m_commandBuffer->commit();
		m_commandBuffer->waitUntilCompleted();
	}

	void MetalContext::draw()
	{
		for (const auto& layer : m_metalLayers)
		{
			m_drawable = NS::TransferPtr(layer->nextDrawable());
			if (m_drawable)
			{
				sendRenderCommands();
			}
		}
	}

	void MetalContext::compute()
	{
		if (!m_computeShaders.empty())
		{
			sendComputeCommands();
		}
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

	void MetalContext::addShader(const std::shared_ptr<GraphicsShader>& metalShader)
	{
		m_graphicsShaders.push_back(metalShader);
	}

	void MetalContext::addShader(const std::shared_ptr<ComputeShader>& metalShader)
	{
		m_computeShaders.push_back(metalShader);
	}
}
