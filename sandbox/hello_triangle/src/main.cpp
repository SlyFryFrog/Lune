#include <iostream>
import lune;


int main()
{
	const lune::WindowCreateInfo windowCreateInfo = {
		.width = 1280,
		.height = 720,
		.title = "Lune: Hello Triangle"
	};

	lune::raii::Window window(windowCreateInfo);
	window.show();

	lune::GraphicsContext* context = &lune::GraphicsContext::instance();

	if (!context)
	{
		std::cerr << "Failed to create graphics context\n";
		return 1;
	}


	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
		{
			window.setShouldClose(true);
		}
		lune::Window::pollEvents();
		context->render();
	}

	return 0;
}
