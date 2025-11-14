module;
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <simd/vector_types.h>
#include <vector>
export module lune:metal_context;

import :graphics_context;
import :metal_shader;

namespace lune::metal
{
	struct TriangleData
	{
		vector_float3 positions[3];
	};

	TriangleData triangle = {{
		{0.0f, 0.5f, 0.0f},   // Top vertex
		{-0.5f, -0.5f, 0.0f}, // Bottom-left vertex
		{0.5f, -0.5f, 0.0f}   // Bottom-right vertex
	}};

	export struct MetalContextCreateInfo
	{
		MTL::ClearColor clearColor = {0.0f, 0.0f, 0.0f, 0.0f};
		MTL::PixelFormat colorPixelFormat = MTL::PixelFormat::PixelFormatBGRA8Unorm;
		int maxFramesInFlight = 3;
	};

	export class MetalContext final : public GraphicsContext
	{
		NS::SharedPtr<MTL::Device> m_device{};
		NS::SharedPtr<MTL::CommandQueue> m_commandQueue{};
		NS::SharedPtr<MTL::CommandBuffer> m_commandBuffer{};
		NS::SharedPtr<CA::MetalDrawable> m_drawable{};
		NS::SharedPtr<MTL::RenderCommandEncoder> m_commandEncoder{};
		std::vector<NS::SharedPtr<CA::MetalLayer>> m_metalLayers{};
		std::vector<std::shared_ptr<GraphicsShader>> m_graphicsShader{};
		MetalContextCreateInfo m_createInfo{};

	private:
		MetalContext();
		~MetalContext() override = default;

	public:
		MetalContext(const MetalContext&) = delete;
		MetalContext& operator=(const MetalContext&) = delete;

		static MetalContext& instance();
		void create(const MetalContextCreateInfo& info);

		[[nodiscard]] MTL::Device* device() const
		{
			return m_device.get();
		}

		[[nodiscard]] MTL::CommandQueue* commandQueue() const
		{
			return m_commandQueue.get();
		}

		[[nodiscard]] std::vector<NS::SharedPtr<CA::MetalLayer>> metalLayers() const
		{
			return m_metalLayers;
		}

		[[nodiscard]] MTL::RenderCommandEncoder* currentRenderEncoder() const
		{
			return m_commandEncoder.get();
		}

		[[nodiscard]] MetalContextCreateInfo& createInfo()
		{
			return m_createInfo;
		}

		void createDevice();
		void createCommandQueue();

		void sendRenderCommand();
		void draw();
		void render() override;

		void addMetalLayer(const NS::SharedPtr<CA::MetalLayer>& metalLayer);
		void removeMetalLayer(const NS::SharedPtr<CA::MetalLayer>& metalLayer);

		void addShader(const std::shared_ptr<GraphicsShader>& metalShader);
	};
} // namespace lune::metal
