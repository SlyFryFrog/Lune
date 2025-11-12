import lune;


int main()
{
	// Describe window characteristics before create instance
	const lune::WindowCreateInfo windowCreateInfo = {
		.width = 1280,
		.height = 720,
		.title = "Lune: Hello Triangle - Metal Renderer",
		.resizable = true,
	};

	const lune::raii::Window window(windowCreateInfo);
	window.show();
	lune::metal::MetalContext& context = lune::metal::MetalContext::instance();

	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
		{
			window.setShouldClose(true);
		}

		context.draw();

		lune::Window::pollEvents();
	}

	return 0;
}
