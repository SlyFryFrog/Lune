import lune;

constexpr lune::Vec2 verticesA[] = {
	{-1.f, -1.f}, {1.f, -1.f}, {-1.f, 1.f},
	{-1.f, 1.f}, {1.f, -1.f}, {1.f, 1.f}
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

	lune::metal::Texture texture({});
	texture.load("shaders/img.png");

	lune::metal::RenderPass pass{};

	window.show();
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		const auto drawable = window.nextDrawable();
		pass.begin(drawable);
		{
			materialA.setUniform("verts", verticesA)
			         .setUniform("tex", texture.texture());
			pass.bind(materialA);
			pass.bind(pipeline);
			pass.draw(lune::Triangle, 0, 6);
		}
		pass.end(drawable);
		lune::Window::pollEvents();
	}

	return 0;
}
