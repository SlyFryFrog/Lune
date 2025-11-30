import lune;

constexpr lune::Vec3 vertices[] = {
	{-0.5f, -0.5f, 0.0f},
	{0.5f, -0.5f, 0.0f},
	{0.0f, 0.5f, 0.0f}
};

constexpr lune::Vec3 colors[] = {
	{1.0f, 0.0f, 0.0f}, // Red
	{0.0f, 1.0f, 0.0f}, // Green
	{0.0f, 0.0f, 1.0f}  // Blue
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

	// Define our shader implementation
	lune::metal::GraphicsShader shader{"shaders/basic.metal"};
	lune::metal::GraphicsPipeline pipeline{shader};
	lune::metal::Material material{pipeline};
	lune::metal::RenderPass pass;

	material.setUniform("vertexPositions", vertices)
		.setUniform("vertexColors", colors);


	// Perform our render loop
	window.show();
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		// Defines the render pass
		pass.begin(window.surface())
		    .bind(material)
		    .draw(lune::Triangle, 0, 3)
		    .end();

		lune::Window::pollEvents();
	}

	return 0;
}
