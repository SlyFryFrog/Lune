import lune;

int main()
{
	const lune::WindowCreateInfo windowCreateInfo = {
		.width = 1280,
		.height = 720,
		.title = "Lune: Hello Triangle",
		.resizable = false,
	};

	lune::raii::Window window(windowCreateInfo);
	window.show();

	lune::GraphicsContext& context = lune::GraphicsContext::instance();

	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
		{
			window.setShouldClose(true);
		}

		context.render();

		lune::Window::pollEvents();
	}

	return 0;
}
