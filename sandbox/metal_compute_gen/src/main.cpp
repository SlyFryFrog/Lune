#include <vector>
#include <Metal/Metal.hpp>
#include <simd/vector_types.h>
import lune;

constexpr size_t Width = 1024;
constexpr size_t Height = 728;
constexpr size_t IterationsPerFrame = 1; ///< Number of compute cycles per frame
constexpr float Zoom = 1.0f;             ///< Currently set in shader manually, need to add caller

class LifeVizShader final : public lune::metal::GraphicsShader
{

public:
	explicit LifeVizShader(const lune::metal::GraphicsShaderCreateInfo& info) :
		GraphicsShader(info)
	{
		createFullscreenQuad();
	}

	void setTexture(MTL::Texture* tex)
	{
		m_texture = tex;
	}

	void encodeRenderCommand(MTL::RenderCommandEncoder* enc) override
	{
		enc->setRenderPipelineState(m_pipelineState.get());
		enc->setVertexBuffer(m_vertexBuffer.get(), 0, 0);
		if (m_texture)
			enc->setFragmentTexture(m_texture, 0);
		enc->drawPrimitives(MTL::PrimitiveTypeTriangle, static_cast<NS::UInteger>(0), 6);
	}

private:
	void createFullscreenQuad()
	{
		constexpr simd::float2 quad[] = {
			{-1.f, -1.f}, {1.f, -1.f}, {-1.f, 1.f},
			{-1.f, 1.f}, {1.f, -1.f}, {1.f, 1.f}
		};
		m_vertexBuffer = NS::TransferPtr(
			m_device->newBuffer(&quad, sizeof(quad), MTL::ResourceStorageModeShared));
	}
};


int main()
{
	lune::setWorkingDirectory();

	auto& context = lune::metal::MetalContext::instance();

	// Create visual shader
	lune::metal::GraphicsShaderCreateInfo vizInfo{.path = "shaders/life_visualize.metal"};
	auto vizShader = std::make_shared<LifeVizShader>(vizInfo);
	context.addShader(vizShader);

	// Create window
	const lune::WindowCreateInfo winInfo{
		Width,
		Height,
		"Lune: GPU Game of Life",
		false
	};
	const lune::raii::Window window(winInfo);
	window.show();

	// Create textures
	MTL::TextureDescriptor* desc = MTL::TextureDescriptor::texture2DDescriptor(
		MTL::PixelFormatRGBA8Unorm, Width, Height, false);
	desc->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite);

	// Initialize the shader's texture
	MTL::Texture* shaderTexture = context.device()->newTexture(desc);

	// Generate initial data to be fed into compute shader
	std::vector<uint8_t> pixelData(Width * Height * 4);
	for (size_t i = 0; i < Width * Height; ++i)
	{
		const uint8_t v = rand() & 1 ? 255 : 0;
		pixelData[i * 4 + 0] = v;
		pixelData[i * 4 + 1] = v;
		pixelData[i * 4 + 2] = v;
		pixelData[i * 4 + 3] = v;
	}

	// Create out buffer and copy data
	MTL::Buffer* inBuff = context.device()->newBuffer(pixelData.size(), MTL::StorageModeShared);
	MTL::Buffer* outBuff = context.device()->newBuffer(pixelData.size(), MTL::StorageModeShared);
	std::memcpy(inBuff->contents(), pixelData.data(), pixelData.size());

	// Compute shader setup
	auto computeShader = lune::metal::ComputeShader("shaders/life_compute.metal");
	auto kernel = computeShader.kernel("computeNextStateBuffer")
	                           .setByte("width", Width)
	                           .setByte("height", Height);

	vizShader->setTexture(shaderTexture);

	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);

		if (!lune::InputManager::isOrderedPressed({lune::KEY_W}))
		{
			lune::Window::pollEvents();
			continue;
		}

		// Perform the simulation
		for (size_t i = 0; i < IterationsPerFrame; ++i)
		{
			kernel.setBuffer("inBuff", inBuff)
			      .setBuffer("outBuff", outBuff)
			      .dispatch(Width, Height, 1)
			      .waitUntilComplete();
		}

		// Copy buffer data to texture and then draw
		lune::metal::ComputeKernel::bufferToTexture(outBuff, shaderTexture,
		                                            Width * 4, // RGBA8
		                                            {Width, Height, 1});

		std::swap(inBuff, outBuff); // Not really necessary, could read/write to single buffer

		context.draw();

		lune::Window::pollEvents();
	}

	return 0;
}
