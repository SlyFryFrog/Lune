#include <Metal/Metal.hpp>
import lune;

constexpr lune::Vec3 verticesA[] = {
	{-0.5f, -0.5f, 0.0f},
	{0.5f, -0.5f, 0.0f},
	{0.0f, 0.5f, 0.0f}
};

constexpr lune::Vec3 verticesB[] = {
	{-0.25f, -0.25f, 0.0f},
	{0.25f, -0.25f, 0.0f},
	{0.0f, 0.50f, 0.0f}
};

int main()
{
	lune::setWorkingDirectory();

	const lune::WindowCreateInfo windowCreateInfo = {
		.width = 1280,
		.height = 720,
		.title = "Lune: Sandbox - Metal Renderer",
		.resizable = true,
	};
	const lune::raii::Window window(windowCreateInfo);

	lune::metal::GraphicsShader module{"shaders/basic.metal"};
	lune::metal::GraphicsPipeline pipeline{module};

	lune::metal::Material materialA{pipeline};
	lune::metal::Material materialB{pipeline};

	constexpr lune::Vec4 colorA{182.0f / 255.0f, 0.0f / 255.0f, 228.0f / 255.0f, 1.0f};
	constexpr lune::Vec4 colorB{0.0f, 182.0f / 255.0f, 228.0f / 255.0f, 1.0f};

	lune::metal::RenderPass pass{};

	window.show();
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		const auto drawable = window.nextDrawable();
		pass.begin(drawable);

		materialA.setUniform("vertexPositions", verticesA)
		         .setUniform("uColor", colorA);
		pass.bind(materialA);
		pass.bind(pipeline);
		pass.draw(lune::Triangle, 0, 3);

		materialB.setUniform("vertexPositions", verticesB)
		         .setUniform("uColor", colorB);
		pass.bind(materialB);
		pass.draw(lune::Triangle, 0, 3);

		pass.end(drawable);
		lune::Window::pollEvents();
	}

	return 0;
}
