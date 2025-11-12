module;
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <iostream>
#include <simd/vector_types.h>
module lune;

import :file;

namespace lune::metal
{
	MetalContext::MetalContext()
	{
		createDevice();
		createTriangle();
		createLibrary("/Users/marcus/dev/Lune/sandbox/metal_hello_triangle/shaders/basic.metal");
		createCommandQueue();
		createRenderPipeline();
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
	}

	void MetalContext::createDefaultLibrary()
	{
		m_library = NS::TransferPtr(m_device->newDefaultLibrary());
		if (!m_library)
		{
			throw std::runtime_error("Failed to create default Metal library");
		}
	}

	void MetalContext::createLibrary(const std::string& path)
	{
		// Try to load shader from file on disk
		const auto shaderSource = File::read(path);
		const auto source = NS::String::string(shaderSource.value_or("").c_str(),
		                                       NS::UTF8StringEncoding);

		NS::Error* error;
		m_library = NS::TransferPtr(m_device->newLibrary(source, nullptr, &error));
		if (!m_library)
		{
			throw std::runtime_error("Failed to create Metal library");
		}
	}

	void MetalContext::createCommandQueue()
	{
		m_commandQueue = NS::TransferPtr(m_device->newCommandQueue());
		if (!m_commandQueue)
		{
			throw std::runtime_error("Failed to create Metal command queue");
		}
	}

	void MetalContext::createTriangle()
	{
		const simd::float3 vertices[] = {
			{-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.0f, 0.5f, 0.0f}};

		m_triangleVertexBuffer = NS::TransferPtr(m_device->newBuffer(&vertices, sizeof(vertices),
			MTL::ResourceStorageModeShared));
	}

	void MetalContext::createRenderPipeline()
	{
		const MTL::Function* vertShader = m_library->newFunction(
			NS::String::string("vertexShader", NS::ASCIIStringEncoding));
		assert(vertShader);

		const MTL::Function* fragShader = m_library->newFunction(
			NS::String::string("fragmentShader", NS::ASCIIStringEncoding));
		assert(fragShader);

		MTL::RenderPipelineDescriptor* renderPipelineDescriptor =
			MTL::RenderPipelineDescriptor::alloc()->init();
		renderPipelineDescriptor->setLabel(
			NS::String::string("Triangle Rendering Pipeline", NS::ASCIIStringEncoding));
		renderPipelineDescriptor->setVertexFunction(vertShader);
		renderPipelineDescriptor->setFragmentFunction(fragShader);

		renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatBGRA8Unorm);

		NS::Error* error;
		m_pipelineState = NS::TransferPtr(
			m_device->newRenderPipelineState(renderPipelineDescriptor, &error));

		renderPipelineDescriptor->release();
	}

	void MetalContext::encodeRenderCommand(MTL::RenderCommandEncoder* renderEncoder) const
	{
		renderEncoder->setRenderPipelineState(m_pipelineState.get());
		renderEncoder->setVertexBuffer(m_triangleVertexBuffer.get(), 0, 0);
		constexpr MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
		constexpr NS::UInteger vertexStart = 0;
		constexpr NS::UInteger vertexCount = 3;
		renderEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
	}

	void MetalContext::sendRenderCommand()
	{
		m_commandBuffer = NS::TransferPtr(m_commandQueue->commandBuffer());

		MTL::RenderPassDescriptor* renderPassDescriptor =
			MTL::RenderPassDescriptor::alloc()->init();
		MTL::RenderPassColorAttachmentDescriptor* cd =
			renderPassDescriptor->colorAttachments()->object(0);
		cd->setTexture(m_drawable->texture());
		cd->setLoadAction(MTL::LoadActionClear);
		cd->setClearColor(MTL::ClearColor(0.33, 0.33, 0.33, 1.0));
		cd->setStoreAction(MTL::StoreActionStore);

		MTL::RenderCommandEncoder* renderCommandEncoder = m_commandBuffer->renderCommandEncoder(
			renderPassDescriptor);
		encodeRenderCommand(renderCommandEncoder);
		renderCommandEncoder->endEncoding();

		m_commandBuffer->presentDrawable(m_drawable.get());
		m_commandBuffer->commit();
		m_commandBuffer->waitUntilCompleted();

		renderPassDescriptor->release();
	}

	void MetalContext::draw()
	{
		for (const auto& layer : m_metalLayers)
		{
			m_drawable = NS::TransferPtr(layer->nextDrawable());
			if (m_drawable)
			{
				sendRenderCommand();
			}
		}
	}

	NS::SharedPtr<CA::MetalLayer> MetalContext::createMetalLayer() const
	{
		auto metalLayer = NS::TransferPtr(CA::MetalLayer::layer());
		metalLayer->setDevice(m_device.get());
		metalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
		metalLayer->setFramebufferOnly(false);

		return metalLayer;
	}

	void MetalContext::render()
	{
	}
} // namespace lune::metal
