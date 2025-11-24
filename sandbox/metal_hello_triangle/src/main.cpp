#include <Metal/Metal.hpp>
#include <simd/vector_types.h>

import lune;

constexpr simd::float3 vertices[] = {
	{-0.5f, -0.5f, 0.0f},
	{0.5f, -0.5f, 0.0f},
	{0.0f, 0.5f, 0.0f}
};


int main()
{
	lune::setWorkingDirectory(); // So we can use local paths from executable

	// Initialize our window
	const lune::WindowCreateInfo windowCreateInfo = {
		.width = 1280,
		.height = 720,
		.title = "Lune: Sandbox - Metal Renderer",
		.resizable = true,
	};
	const lune::raii::Window window(windowCreateInfo);

	// Get our renderer's context
	const lune::metal::MetalContext& context = lune::metal::MetalContext::instance();

	const lune::metal::GraphicsShader module{"shaders/basic.metal"};
	lune::metal::GraphicsPipeline pipeline{module};
	lune::metal::RenderPass pass{&pipeline};

	const MTL::Buffer* vertexBuff = context.device()->newBuffer(vertices, sizeof(vertices),
																MTL::ResourceStorageModeShared);

	// Perform our render loop
	window.show();
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		const auto drawable = window.nextDrawable();
		pass.begin(drawable);
		{
			pass.bind(pipeline);
			pass.setVertexBuffer(vertexBuff, 0, 0);
			pass.draw(MTL::PrimitiveTypeTriangle, 3, 0);
		}
		pass.end(drawable);
		lune::Window::pollEvents();
	}

	return 0;
}
