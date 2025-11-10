module;
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include <iostream>
#include <ostream>
#include <simd/simd.h>
#include <AppKit/AppKit.hpp>
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
export module metal_demo;

constexpr int kMaxFramesInFlight = 3;


struct TriangleData
{
	vector_float3 positions[3];
};

TriangleData triangle = {
	{
		{0.0f, 0.5f, 0.0f},   // Top vertex
		{-0.5f, -0.5f, 0.0f}, // Bottom-left vertex
		{0.5f, -0.5f, 0.0f}   // Bottom-right vertex
	}
};


export class MetalDemo
{
	MetalDemo();

	~MetalDemo();
public:
	MTL::Device* m_device{};
	MTL::CommandQueue* m_commandQueue{};
	MTL::CommandBuffer* m_commandBuffer{};
	MTL::Library* m_library{};
	CA::MetalDrawable* m_drawable{};
	CA::MetalLayer* m_layer{};
	MTL::RenderPipelineState* m_renderPSO{};
	MTL::Buffer* m_triangleVertexBuffer{};

	static MetalDemo& instance()
	{
		static MetalDemo s_instance;
		return s_instance;
	}

	void createDevice();
	void createDefaultLibrary();
	void createCommandQueue();
	void createTriangle();
	void createRenderPipeline();

	void encodeRenderCommand(MTL::RenderCommandEncoder* renderEncoder) const;
	void sendRenderCommand();
	void draw();
};

MetalDemo::MetalDemo()
{
	createDevice();

	createTriangle();
	createDefaultLibrary();
	createCommandQueue();
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
    return float4(182.0/255.0, 240.0/255.0, 228.0/255.0, 1.0);
}
)";


		const auto path = NS::String::string("default.metallib", NS::ASCIIStringEncoding);

		NS::Error* error = nullptr;
		// m_library = m_device->newLibrary(path, &error);
		auto source = NS::String::string(shaderSource, NS::UTF8StringEncoding);
		m_library = m_device->newLibrary(source, nullptr, &error);

		if (!m_library)
		{
			printf("Failed to load metallib: %s\n", error->localizedDescription()->utf8String());
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
		{-0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{0.0f, 0.5f, 0.0f}
	};

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

	MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()
		->init();
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

	MTL::RenderPassDescriptor* renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
	MTL::RenderPassColorAttachmentDescriptor* cd = renderPassDescriptor->colorAttachments()->
		object(0);
	cd->setTexture(m_drawable->texture());
	cd->setLoadAction(MTL::LoadActionClear);
	cd->setClearColor(MTL::ClearColor(41.0f / 255.0f, 42.0f / 255.0f, 48.0f / 255.0f, 1.0));
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
