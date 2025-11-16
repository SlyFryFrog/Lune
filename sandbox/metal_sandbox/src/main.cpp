#include <Metal/Metal.hpp>
#include <simd/vector_types.h>

import lune;


class CustomShader final : public lune::metal::GraphicsShader
{
public:
	explicit CustomShader(const lune::metal::GraphicsShaderCreateInfo& createInfo) :
		GraphicsShader(createInfo)
	{
		createVertices();
	}

	void encodeRenderCommand(MTL::RenderCommandEncoder* renderCommandEncoder) override
	{
		renderCommandEncoder->setRenderPipelineState(m_pipelineState.get());
		renderCommandEncoder->setVertexBuffer(m_vertexBuffer.get(), 0, 0);
		constexpr MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
		constexpr NS::UInteger vertexStart = 0;
		constexpr NS::UInteger vertexCount = 3;
		renderCommandEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
	}

	void setupPipelineDescriptor(MTL::RenderPipelineDescriptor* pipelineDescriptor) override
	{
		// Add custom implementation if desired
	}

private:
	void createVertices()
	{
		constexpr simd::float3 vertices[] = {
			{-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.0f, 0.5f, 0.0f}};

		m_vertexBuffer = NS::TransferPtr(m_device->newBuffer(&vertices, sizeof(vertices),
		                                                     MTL::ResourceStorageModeShared));
	}
};


int main()
{
	lune::setWorkingDirectory(); // So we can use local paths from executable

	// Initialize our metal renderer and optionally modify defaults
	const lune::metal::MetalContextCreateInfo metalContextCreateInfo = {
		.clearColor = {0.33, 0.33, 0.66, 1.0},
		.maxFramesInFlight = 3,
	};

	lune::metal::MetalContext& context = lune::metal::MetalContext::instance();
	context.create(metalContextCreateInfo);

	// Add our custom shader to the renderer
	lune::metal::GraphicsShaderCreateInfo shaderCreateInfo = {
		.path = "shaders/slang-basic.metal", // Must be set for each createInfo struct
	};
	const auto shader = std::make_shared<CustomShader>(shaderCreateInfo);
	context.addShader(shader);

	// Initialize our window
	const lune::WindowCreateInfo windowCreateInfo = {
		.width = 1280,
		.height = 720,
		.title = "Lune: Sandbox - Metal Renderer",
		.resizable = true,
	};

	const lune::raii::Window window(windowCreateInfo);
	window.show();

	// Perform our render loop
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		// Reload shader
		if (lune::InputManager::isJustPressed(lune::KEY_R))
		{
			shader->createRenderPipeline();
			shader->loadFunctions();
		}

		context.draw();

		lune::Window::pollEvents();
	}

	return 0;
}
