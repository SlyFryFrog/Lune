#include <iostream>

import lune;


void attachMetalToGLFW(lune::raii::Window& window);


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

	window.attachMetalToGLFW();

	// lune::GraphicsContext* context = &lune::GraphicsContext::instance();
	//
	// if (!context)
	// {
	// 	std::cerr << "Failed to create graphics context\n";
	// 	return 1;
	// }

	lune::metal::MetalContext& context = lune::metal::MetalContext::instance();
	context.createMetalLayer(800, 600);
	context.setupVertexBuffer();
	context.setupPipeline();


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
