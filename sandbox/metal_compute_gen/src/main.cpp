#include <array>
#include <Metal/Metal.hpp>
import lune;

constexpr size_t Width = 1024;
constexpr size_t Height = 728;
constexpr size_t IterationsPerFrame = 10; ///< Number of compute cycles per frame
constexpr float Zoom = 0.25f;
constexpr uint8_t CellColor[] = {255, 0, 0, 255};
constexpr uint8_t BackgroundColor[] = {25, 25, 25, 255};
constexpr lune::Vec2 quad[] = {
	{-1.f, -1.f}, {1.f, -1.f}, {-1.f, 1.f},
	{-1.f, 1.f}, {1.f, -1.f}, {1.f, 1.f}
};

int main()
{
	lune::setWorkingDirectory();

	auto& context = lune::metal::MetalContext::instance();

	// Create window
	const lune::WindowCreateInfo winInfo{
		Width,
		Height,
		"Lune – Jeu de la Vie sur GPU",
		false
	};
	const lune::raii::Window window(winInfo);

	// Create our texture
	lune::TextureContextCreateInfo textureContextInfo{
		.pixelFormat = lune::PixelFormat::RGBA8_UNorm,
		.width = Width,
		.height = Height,
		.mipmapped = false,
	};
	lune::Texture texture = context.createTexture(textureContextInfo);

	// Generate initial data to be fed into compute shader
	std::array<uint8_t, Width * Height * 4> pixelData{};
	for (size_t i = 0; i < Width * Height; ++i)
	{
		const uint8_t v = rand() & 1 ? 255 : 0;
		pixelData[i * 4 + 0] = v;
		pixelData[i * 4 + 1] = v;
		pixelData[i * 4 + 2] = v;
		pixelData[i * 4 + 3] = v;
	}

	// Create out buffer and copy data
	lune::Buffer inBuff = context.createBuffer(pixelData.size());
	lune::Buffer outBuff = context.createBuffer(pixelData.size());
	std::memcpy(inBuff.data(), pixelData.data(), pixelData.size());

	// Compute shader setup
	auto computeShader = lune::metal::ComputeShader("shaders/life_compute.metal");
	auto kernel = computeShader.kernel("computeNextStateBuffer")
	                           .setUniform("width", Width)
	                           .setUniform("height", Height)
	                           .setUniform("cellColor", CellColor)
	                           .setUniform("backgroundColor", BackgroundColor);

	lune::metal::GraphicsShader shader{
		"shaders/life_visualize.metal",
		"vertexMain",
		"fragmentMain",
	};
	lune::metal::GraphicsPipeline pipeline{shader};
	lune::metal::Material material{pipeline};
	lune::metal::RenderPass pass{};

	material.setUniform("verts", quad)
	        .setUniform("zoom", Zoom);

	window.show();
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		if (!lune::InputManager::isPressed(lune::KEY_W))
		{
			lune::Window::pollEvents();
			continue;
		}

		// Perform the simulation
		for (size_t i = 0; i < IterationsPerFrame; ++i)
		{
			kernel.setUniform("inBuff", inBuff)
			      .setUniform("outBuff", outBuff)
			      .dispatch(Width, Height, 1)
			      .waitUntilComplete();

			// Since we are copying data on CPU, we need to wait for GPU results
			// If we instead swap ptrs to buffers we don't need to wait
			inBuff.update(outBuff.data(), outBuff.size());
		}

		// Copy buffer data to texture and then draw
		lune::metal::ComputeKernel::bufferToTexture(inBuff, texture,
		                                            Width * 4, // RGBA8
		                                            {Width, Height, 1});


		material.setUniform("tex", texture);

		pass.begin(window.surface())
		    .bind(material)
		    .draw(lune::Triangle, 0, 6)
		    .end();

		lune::Window::pollEvents();
	}

	return 0;
}
