#include <Metal/Metal.hpp>
import lune;

constexpr lune::Vec3 vertices[] = {
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

	lune::metal::GraphicsShader module{"shaders/basic.metal"};
	lune::metal::GraphicsPipeline pipeline{module};
	lune::metal::Material material{pipeline};
	lune::metal::RenderPass pass{};

	// Perform our render loop
	window.show();
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		const auto drawable = window.nextDrawable();
		pass.begin(drawable);
		{
			material.setUniform("vertexPositions", vertices, sizeof(vertices));
			pass.bind(material);
			pass.bind(pipeline);
			pass.draw(MTL::PrimitiveTypeTriangle, 0, 3);
		}
		pass.end(drawable);
		lune::Window::pollEvents();
	}

	return 0;
}
