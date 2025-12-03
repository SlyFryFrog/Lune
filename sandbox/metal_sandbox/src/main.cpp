import lune;

constexpr lune::Vec2 verticesA[] = {
	{-1.f, -1.f}, {1.f, -1.f}, {-1.f, 1.f},
	{-1.f, 1.f}, {1.f, -1.f}, {1.f, 1.f}
};


int main()
{
	lune::setWorkingDirectory();

	auto& context = lune::GraphicsContext::instance();

	const lune::WindowCreateInfo windowCreateInfo = {
		.width = 1280,
		.height = 720,
		.title = "Lune: Sandbox - Metal Renderer",
		.resizable = true,
	};
	const lune::raii::Window window(windowCreateInfo);

	lune::metal::GraphicsShader module{"shaders/basic.metal"};
	lune::metal::GraphicsPipeline pipeline{module};
	lune::metal::RenderPass pass{};

	lune::metal::Material materialA{pipeline};

	lune::Texture texture = context.createTexture({});
	texture.load("shaders/img.png");


	materialA.setUniform("verts", verticesA)
	         .setUniform("tex", texture);


	window.show();
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		pass.begin(window.surface())
		    .bind(materialA)
		    .draw(lune::Triangle, 0, 6)
		    .end();
		lune::Window::pollEvents();
	}

	return 0;
}
