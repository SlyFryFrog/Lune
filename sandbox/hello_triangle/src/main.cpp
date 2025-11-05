#include <iostream>

#include <Metal/MTLRenderPass.hpp>

import lune;

class CustomLayer final : public lune::metal::MetalLayer
{
public:
	CustomLayer() = default;
	~CustomLayer() override = default;

	void setup() override
	{
	}

	void render() override
	{
	}
};


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

	lune::metal::MetalContext& context = lune::metal::MetalContext::instance();
	context.addLayer<CustomLayer>();


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
