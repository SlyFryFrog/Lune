import lune;

int main()
{
	const lune::WindowCreateInfo windowCreateInfo = {
		.width = 1280,
		.height = 720,
		.title = "Lune: Hello Window",
		.resizable = false,
	};

	lune::raii::Window window(windowCreateInfo);
	window.show();

	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		lune::Window::pollEvents();
	}

	return 0;
}
