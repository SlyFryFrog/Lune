#include <iostream>
#include <array>

#include <AppKit/AppKit.hpp>
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <simd/vector_types.h>

import lune;


simd::float3 positions[] =
{
	{-0.8f, 0.8f, 0.0f},
	{0.0f, -0.8f, 0.0f},
	{+0.8f, 0.8f, 0.0f}
};

simd::float3 colors[] =
{
	{1.0, 0.3f, 0.2f},
	{0.8f, 1.0, 0.0f},
	{0.8f, 0.0f, 1.0}
};


class CustomLayer final : public lune::metal::MetalLayer
{
	std::vector<NS::SharedPtr<MTL::Buffer>> makeTriangleDataBuffers(NS::UInteger count);
	NS::SharedPtr<MTL4::ArgumentTable> makeArgumentTable();
	NS::SharedPtr<MTL::Buffer> makeResidencySet();
	std::vector<NS::SharedPtr<MTL4::CommandBuffer>> makeCommandAllocators(NS::UInteger count);

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

std::vector<NS::SharedPtr<MTL::Buffer>> CustomLayer::makeTriangleDataBuffers(NS::UInteger count)
{
	std::vector<NS::SharedPtr<MTL::Buffer>> buffers;

	for (NS::UInteger i = 0; i < count; ++i)
	{
		// Allocate and fill a buffer with triangle data
		float vertices[] = {

		};
		NS::SharedPtr<MTL::Buffer> buffer = NS::TransferPtr(
			lune::metal::MetalContext::instance().device()->
			                                      newBuffer(
				                                      sizeof(vertices),
				                                      MTL::ResourceStorageModeShared));
		memcpy(buffer->contents(), vertices, sizeof(vertices));
		buffers.push_back(buffer);
	}
	return buffers;
}

NS::SharedPtr<MTL4::ArgumentTable> CustomLayer::makeArgumentTable()
{
	NS::Error* error = nullptr;

	// Configure the settings for a new argument table with two buffer bindings.
	NS::SharedPtr<MTL4::ArgumentTableDescriptor> argumentTableDescriptor = NS::TransferPtr(
		MTL4::ArgumentTableDescriptor::alloc()->init());

	argumentTableDescriptor->setMaxBufferBindCount(2);


	// Create the argument table.
	NS::SharedPtr<MTL4::ArgumentTable> argumentTable = NS::TransferPtr(
		lune::metal::MetalContext::instance().device()->newArgumentTable(
			argumentTableDescriptor.get(), &error));

	if (error)
	{
		std::cout << "Error creating argumentTable: " << error << std::endl;
	}

	return argumentTable;
}

NS::SharedPtr<MTL::Buffer> CustomLayer::makeResidencySet()
{
}

std::vector<NS::SharedPtr<MTL4::CommandBuffer>> CustomLayer::makeCommandAllocators(
	NS::UInteger count)
{
}


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

	// lune::metal::MetalContext& context = lune::metal::MetalContext::instance();
	// context.addLayer<CustomLayer>();

	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
		{
			window.setShouldClose(true);
		}

		// context.render();

		lune::metal::MetalDemo::instance().draw();

		lune::Window::pollEvents();
	}

	return 0;
}
