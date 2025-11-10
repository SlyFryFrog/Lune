module;
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <iostream>
#include <simd/vector_types.h>
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
	}

	void MetalContext::createDefaultLibrary()
	{
		m_library = NS::TransferPtr(m_device->newDefaultLibrary());
		if (!m_library)
		{
			throw std::runtime_error("Failed to create default Metal library");
		}
	}

	void MetalContext::createLibrary()
	{
		const char* shaderSource = R"(
#include <metal_stdlib>
using namespace metal;

struct VertexOut {
    float4 position [[position]];
};

vertex VertexOut vertexShader(const device float3* vertexPositions [[buffer(0)]],
                              uint vertexID [[vertex_id]])
{
    VertexOut out;
    out.position = float4(vertexPositions[vertexID], 1.0);
    return out;
}

fragment float4 fragmentShader(VertexOut in [[stage_in]])
{
    return float4(1.0, 0.0, 1.0, 1.0);
}
)";
		auto source = NS::String::string(shaderSource, NS::UTF8StringEncoding);

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

		m_triangleVertexBuffer = m_device->newBuffer(&vertices, sizeof(vertices),
													 MTL::ResourceStorageModeShared);
	}

	void MetalContext::createRenderPipeline()
	{
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

	MetalDemo::MetalDemo()
	{
		createDevice();

		createTriangle();
		createDefaultLibrary();
		createCommandQueue();
		m_layer = CA::MetalLayer::layer();
		m_layer->setDevice(m_device);
		m_layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
		m_layer->setFramebufferOnly(false);

		createRenderPipeline();
	}

	MetalDemo::~MetalDemo()
	{
		m_device->release();
		m_commandQueue->release();
		m_commandBuffer->release();
		m_library->release();
	}

	void MetalDemo::createDevice()
	{
		m_device = MTL::CreateSystemDefaultDevice();
		assert(m_device);
	}

	void MetalDemo::createDefaultLibrary()
	{
		m_library = m_device->newDefaultLibrary();
		if (!m_library)
		{
			const char* shaderSource = R"(
#include <metal_stdlib>
using namespace metal;

struct VertexOut {
    float4 position [[position]];
};

vertex VertexOut vertexShader(const device float3* vertexPositions [[buffer(0)]],
                              uint vertexID [[vertex_id]])
{
    VertexOut out;
    out.position = float4(vertexPositions[vertexID], 1.0);
    return out;
}

fragment float4 fragmentShader(VertexOut in [[stage_in]])
{
    return float4(1.0, 0.0, 1.0, 1.0);
}
)";


			const auto path = NS::String::string("default.metallib", NS::ASCIIStringEncoding);

			NS::Error* error = nullptr;
			// m_library = m_device->newLibrary(path, &error);
			auto source = NS::String::string(shaderSource, NS::UTF8StringEncoding);
			m_library = m_device->newLibrary(source, nullptr, &error);

			if (!m_library)
			{
				printf("Failed to load metallib: %s\n",
					   error->localizedDescription()->utf8String());
			}
		}
	}

	void MetalDemo::createCommandQueue()
	{
		m_commandQueue = m_device->newCommandQueue();
	}

	void MetalDemo::createTriangle()
	{
		const simd::float3 vertices[] = {
			{-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.0f, 0.5f, 0.0f}};

		m_triangleVertexBuffer = m_device->newBuffer(&vertices, sizeof(vertices),
													 MTL::ResourceStorageModeShared);
	}

	void MetalDemo::createRenderPipeline()
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
		m_renderPSO = m_device->newRenderPipelineState(renderPipelineDescriptor, &error);

		renderPipelineDescriptor->release();
	}

	void MetalDemo::draw()
	{
		m_drawable = m_layer->nextDrawable();
		if (!m_drawable)
		{
			std::cout << "nextDrawable() returned nil\n";
		}

		sendRenderCommand();
	}

	void MetalDemo::sendRenderCommand()
	{
		m_commandBuffer = m_commandQueue->commandBuffer();

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

		m_commandBuffer->presentDrawable(m_drawable);
		m_commandBuffer->commit();
		m_commandBuffer->waitUntilCompleted();

		renderPassDescriptor->release();
	}

	void MetalDemo::encodeRenderCommand(MTL::RenderCommandEncoder* renderEncoder) const
	{
		renderEncoder->setRenderPipelineState(m_renderPSO);
		renderEncoder->setVertexBuffer(m_triangleVertexBuffer, 0, 0);
		constexpr MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
		constexpr NS::UInteger vertexStart = 0;
		constexpr NS::UInteger vertexCount = 3;
		renderEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
	}
} // namespace lune::metal
