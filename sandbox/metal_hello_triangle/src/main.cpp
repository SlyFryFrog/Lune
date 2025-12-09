import lune;

constexpr lune::Vec3 verticesB[]{
		{-0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{0.0f, 0.5f, 0.0f},
};

constexpr lune::Vec3 colors[]{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
};


int main()
{
	lune::setWorkingDirectory(); // So we can use local paths from executable

	const lune::raii::Window window{{
			.width = 1280,
			.height = 720,
			.title = "Lune: Sandbox - Metal Renderer",
			.resizable = true,
	}};

	// Define our shader implementation
	lune::metal::GraphicsShader shader{"shaders/basic.metal"};
	lune::metal::GraphicsPipeline pipeline{shader};
	lune::metal::RenderPass pass{window.surface()};

	// Create our material - used to set our uniforms
	lune::metal::Material material{pipeline};
	material.setUniform("vertexPositions", verticesB).setUniform("vertexColors", colors);

	// Perform our render loop
	window.show();
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		// Defines the render pass
		pass.begin().bind(material).draw(lune::gfx::Triangle, 0, 3).end();

		lune::Window::pollEvents();
	}

	return 0;
}
