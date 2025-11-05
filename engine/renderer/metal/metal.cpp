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

	void MetalContext::setupVertexBuffer()
	{
		const Vertex vertices[] = {
			{{0.0f, 0.5f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},   // Top
			{{-0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // Bottom-left
			{{0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}   // Bottom-right
		};

		m_vertexBuffer = NS::TransferPtr(
			m_device->newBuffer(
				sizeof(vertices),
				MTL::ResourceStorageModeShared
				)
			);
		memcpy(m_vertexBuffer->contents(), vertices, sizeof(vertices));
	}

	void MetalContext::setupPipeline()
	{
		const char* shaderSource = R"(
    #include <metal_stdlib>
    using namespace metal;

    struct VertexIn {
        float4 position [[attribute(0)]];
        float4 color [[attribute(1)]];
    };

    struct RasterizerData {
        float4 position [[position]];
        float4 color;
    };

    vertex RasterizerData vertexShader(
        const device VertexIn* vertexArray [[buffer(0)]],
        unsigned int vid [[vertex_id]]
    ) {
        RasterizerData out;
        out.position = vertexArray[vid].position;
        out.color = vertexArray[vid].color;
        return out;
    }

    fragment float4 fragmentShader(RasterizerData in [[stage_in]])
    {
        return in.color;
    }
)";


		NS::String* shaderString = NS::String::string(shaderSource, NS::UTF8StringEncoding);
		NS::Error* error = nullptr;
		m_library = NS::TransferPtr(m_device->newLibrary(shaderString, nullptr, &error));

		if (!m_library)
		{
			NS::String* errorDescription = error->localizedDescription();
			const char* errorCStr = errorDescription->utf8String();
			std::cerr << "Failed to create Metal shader library: " << errorCStr << std::endl;
			throw std::runtime_error("Failed to create Metal shader library");
		}

		MTL::Function* vertexFunction = m_library->newFunction(
			NS::String::string("vertexShader", NS::UTF8StringEncoding));
		if (!vertexFunction)
		{
			throw std::runtime_error("Failed to create vertex function");
		}

		MTL::Function* fragmentFunction = m_library->newFunction(
			NS::String::string("fragmentShader", NS::UTF8StringEncoding));
		if (!fragmentFunction)
		{
			vertexFunction->release();
			throw std::runtime_error("Failed to create fragment function");
		}

		MTL::RenderPipelineDescriptor* pipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->
			init();
		pipelineDescriptor->setVertexFunction(vertexFunction);
		pipelineDescriptor->setFragmentFunction(fragmentFunction);
		pipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(
			MTL::PixelFormatBGRA8Unorm);

		m_pipelineState = NS::TransferPtr(
			m_device->newRenderPipelineState(pipelineDescriptor, &error));
		if (!m_pipelineState)
		{
			NS::String* errorDescription = error->localizedDescription();
			const char* errorCStr = errorDescription->utf8String();
			std::cerr << "Failed to create Metal pipeline state: " << errorCStr << std::endl;
			throw std::runtime_error("Failed to create Metal pipeline state");
		}

		vertexFunction->release();
		fragmentFunction->release();
		pipelineDescriptor->release();
	}

	void MetalContext::draw()
	{
		if (!m_metalLayer)
			return;

		CA::MetalDrawable* drawable = m_metalLayer.get()->nextDrawable();
		if (!drawable)
			return;

		MTL::CommandBuffer* commandBuffer = m_commandQueue.get()->commandBuffer();
		MTL::RenderPassDescriptor* renderPassDescriptor =
			MTL::RenderPassDescriptor::renderPassDescriptor();
		renderPassDescriptor->colorAttachments()->object(0)->setTexture(drawable->texture());
		renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
		renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
		renderPassDescriptor->colorAttachments()->object(0)->setClearColor(
			MTL::ClearColor(0.0, 0.0, 0.0, 1.0));

		MTL::RenderCommandEncoder* renderEncoder = commandBuffer->renderCommandEncoder(
			renderPassDescriptor);
		renderEncoder->setRenderPipelineState(m_pipelineState.get());
		renderEncoder->setVertexBuffer(m_vertexBuffer.get(), 0, 0);
		renderEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0),
		                              NS::UInteger(3));
		renderEncoder->endEncoding();

		commandBuffer->presentDrawable(drawable);
		commandBuffer->commit();
	}
}
