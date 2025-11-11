#include <AppKit/AppKit.hpp>
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <simd/vector_types.h>

import lune;

class CustomLayer final : public lune::metal::MetalLayer
{
	std::vector<NS::SharedPtr<MTL::Buffer>> makeTriangleDataBuffers(NS::UInteger count);
	NS::SharedPtr<MTL4::ArgumentTable> makeArgumentTable();
	NS::SharedPtr<MTL::Buffer> makeResidencySet();
	std::vector<NS::SharedPtr<MTL4::CommandBuffer>> makeCommandAllocators(NS::UInteger count);

public:
	CustomLayer() = default;
	~CustomLayer() override = default;

	void setup() override;
	void render() override;
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
