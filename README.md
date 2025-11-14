# Lune

Lune is a work-in-progress game framework for macOS and Linux. It uses Metal on macOS and Vulkan on both macOS and
Linux. The library is written in modern C++ and encourages RAII for safer and
cleaner code.

**Note: XCode can't be used as it currently doesn't support `.cppm` modules, unlike ninja**

## Feature Goals

- [x] Platform independent input and window management system (w/ GLFW).
- [x] Metal Shading Language (MSL) for macOS.
- [ ] Support for Slang (for cross-platform shaders).
- [ ] Load and render 3D meshes (e.g., OBJ, glTF).
- [ ] A simple rendering API that works the same way on both Metal and Vulkan.
- [ ] Shader hot-reloading for faster development.
- [ ] Basic debugging tools (e.g., wireframe mode, GPU profiling).
- [ ] ImGui support for in-engine UI and debugging.

## Requirements and Dependencies

The Metal backend uses a modified (unofficial) version of metal-cpp found in metal-cpp-extensions. These libraries in
metal-cpp-extensions are necessary for interacting with the native window when using GLFW.

| Tool        | macOS (26.0+)          | Linux (Ubuntu 24.04+)          |
|-------------|------------------------|--------------------------------|
| Ninja       | `brew install ninja`   | `sudo apt install ninja-build` |
| CMake       | `brew install cmake`   | `sudo apt install cmake`       |
| Clang++-20+ | `brew install llvm@20` | `sudo apt install clang-20`    |
| Metal       | Metal 4                | N/A                            |
| Vulkan      | Vulkan 1.4             | Vulkan 1.4                     |

| Library   | Description       | License     |
|-----------|-------------------|-------------|
| GLFW3     | Window Management | zlib/libpng |
| GLM       | Mathematics       | MIT         |
| stb_image | Image Loading     | MIT         |

## CMake

### Flags

- BUILD_SANDBOX: Build the sandbox demo projects
- USE_METAL: Build with Metal (macOS)
- USE_VULKAN: Build with Vulkan (All platforms)

## Examples

### Drawing a Triangle in 80 Lines with Metal

Much of the setup regarding the renderer and windowing system is abstracted allowing you to focus on creating your
custom render loops. To define your own render loop, we need to create our own shader class and implement
`encodeRenderCommand`.

```c++
#include <Metal/Metal.hpp>
#include <simd/vector_types.h>

import lune;


class CustomShader final : public lune::metal::GraphicsShader
{
public:
	CustomShader(const std::string& path, const std::string& vertexMain,
	             const std::string& fragmentMain) :
		GraphicsShader(path, vertexMain, fragmentMain)
	{
		createVertices();
	}

	void encodeRenderCommand(MTL::RenderCommandEncoder* renderCommandEncoder) override
	{
		renderCommandEncoder->setRenderPipelineState(m_pipelineState.get());
		renderCommandEncoder->setVertexBuffer(m_vertexBuffer.get(), 0, 0);
		constexpr MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
		constexpr NS::UInteger vertexStart = 0;
		constexpr NS::UInteger vertexCount = 3;
		renderCommandEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
	}

	void createVertices()
	{
		const auto device = lune::metal::MetalContext::instance().device();
		const simd::float3 vertices[] = {
			{-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.0f, 0.5f, 0.0f}};

		m_vertexBuffer = NS::TransferPtr(device->newBuffer(&vertices, sizeof(vertices),
		                                             MTL::ResourceStorageModeShared));
	}
};


int main()
{
	lune::setWorkingDirectory();	// So we can use local paths from executable

	// Initialize our metal renderer
	// This is optional, but it allows you to modify the defaults
	const lune::metal::MetalContextCreateInfo metalContextCreateInfo = {
		.clearColor = {0.33, 0.33, 0.66, 1.0},
		.colorPixelFormat = MTL::PixelFormat::PixelFormatBGRA8Unorm,
		.maxFramesInFlight = 3,
	};

	lune::metal::MetalContext& context = lune::metal::MetalContext::instance();
	context.create(metalContextCreateInfo);

	// Add our custom shader to the renderer
	const auto shader = std::make_shared<CustomShader>(
		"shaders/basic.metal",
		"vertexShader", "fragmentShader");

	context.addShader(shader);

	// Initialize our window
	const lune::WindowCreateInfo windowCreateInfo = {
		.width = 1280,
		.height = 720,
		.title = "Lune: Hello Triangle - Metal Renderer",
		.resizable = true,
	};

	const lune::raii::Window window(windowCreateInfo);
	window.show();

	// Perform our render loop
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		context.draw();

		lune::Window::pollEvents();
	}

	return 0;
}
```