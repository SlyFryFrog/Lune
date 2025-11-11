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
		m_layer = NS::TransferPtr(CA::MetalLayer::layer());
		m_layer->setDevice(m_device.get());
		m_layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
		m_layer->setFramebufferOnly(false);

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

		const auto pixelFormat = m_layer->pixelFormat();
		renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(pixelFormat);

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
		m_drawable = NS::TransferPtr(m_layer->nextDrawable());
		if (!m_drawable)
		{
			std::cout << "nextDrawable() returned nil\n";
			return;
		}

		sendRenderCommand();
	}

	CA::MetalLayer* MetalContext::createMetalLayer(const double width, const double height)
	{
		m_layer = NS::TransferPtr(CA::MetalLayer::layer());
		m_layer->setDevice(m_device.get());
		m_layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
		m_layer->setFramebufferOnly(false);
		m_layer->setDrawableSize(CGSize{width, height});
		return m_layer.get();
	}

	void MetalContext::render()
	{
		if (!m_layer || !m_commandQueue)
			return;

		// Get the next drawable
		const auto drawable = m_layer->nextDrawable();
		if (!drawable)
			return;

		// Create a render pass descriptor
		const auto renderPassDescriptor = MTL::RenderPassDescriptor::renderPassDescriptor();
		const auto colorAttachment = renderPassDescriptor->colorAttachments()->object(0);
		colorAttachment->setClearColor(MTL::ClearColor::Make(0.33, 0.33, 0.33, 1.0));
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
} // namespace lune::metal
