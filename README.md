# Lune

Lune is a work-in-progress game framework for macOS and Linux. It uses Metal on macOS and Vulkan on both macOS and
Linux. The library is written in modern C++ and encourages RAII for safer and
cleaner code.

**Note: Xcode/Xmake can't be used as it currently doesn't support `.cppm` modules, unlike ninja**

## Feature Goals

- [x] Platform independent input and window management system (w/ GLFW).
- [x] Metal Shading Language (MSL) for macOS.
- [ ] Support for Slang (for cross-platform shaders).
- [ ] Support for compute and graphic-related shaders:
    - [x] Metal compute shaders.
    - [x] Metal vertex and fragment shaders.
    - [ ] Vulkan compute shaders.
    - [ ] Vulkan vertex and fragment shaders.
- [ ] Load and render 3D meshes (e.g., OBJ, glTF).
- [ ] A simple rendering API that works the same way on both Metal and Vulkan.
- [ ] Shader hot-reloading for faster development.
- [ ] Support for multiple render passes per-frame.

## Requirements and Dependencies

| Tool        | macOS (26.0+)          | Linux (Ubuntu 24.04+)          |
|-------------|------------------------|--------------------------------|
| Ninja       | `brew install ninja`   | `sudo apt install ninja-build` |
| CMake4.0+   | `brew install cmake`   | `sudo apt install cmake`       |
| Clang++-20+ | `brew install llvm@20` | `sudo apt install clang-20`    |
| Metal       | Metal 4                | N/A                            |
| Vulkan      | Vulkan 1.4             | Vulkan 1.4                     |

## CMake

### Flags

- BUILD_SANDBOX: Build the sandbox demo projects
- USE_METAL: Build with Metal (macOS)
- USE_VULKAN: Build with Vulkan (All platforms)

## Examples

### Drawing a triangle with Metal

Much of the setup regarding the renderer and windowing system is abstracted allowing you to focus on creating your
custom render loops. To define your own render loop, we need to create 4 variables: `GraphicsShader`,
`GraphicsPipeline`, `Material`, and `Renderpass`. These separate the tasks related to drawing to the screen to still give as
much control as possible while still improving usability.

```c++
import lune;

constexpr lune::Vec3 verticesB[]{
		{-0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{0.0f, 0.5f, 0.0f},
};

constexpr lune::Vec3 colors[]{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
};


int main()
{
	lune::setWorkingDirectory(); // So we can use local paths from executable

	const lune::raii::Window window{{
			.width = 1280,
			.height = 720,
			.title = "Lune: Sandbox - Metal Renderer",
			.resizable = true,
	}};

	// Define our shader implementation
	const lune::gfx::Context ctx{};
	const lune::gfx::Shader shader{ctx.createShader({"shaders/basic.metal"})};
	const lune::gfx::Pipeline pipeline{ctx.createPipeline(shader, {})};
	lune::gfx::RenderPass pass{ctx.createRenderPass(window.surface())};

	// Create our material - used to set our uniforms
	lune::gfx::Material material{ctx.createMaterial(pipeline)};
	material.setUniform("vertexPositions", verticesB).setUniform("vertexColors", colors);

	// Perform our render loop
	window.show();
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		// Defines the render pass
		pass.begin().bind(material).draw(lune::gfx::Triangle, 0, 3).end();

		lune::Window::pollEvents();
	}

	return 0;
}
```

## Dependencies

| Library   | Description       | License     |
|-----------|-------------------|-------------|
| GLFW3     | Window Management | zlib/libpng |
| stb_image | Image Loading     | MIT         |
