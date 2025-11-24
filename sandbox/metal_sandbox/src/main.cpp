import lune;

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
	window.show();

	// Perform our render loop
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		lune::Window::pollEvents();
	}

	return 0;
}
