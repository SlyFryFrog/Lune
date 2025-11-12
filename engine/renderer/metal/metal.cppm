module;
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include <AppKit/AppKit.hpp>
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <simd/vector_types.h>
#include <vector>
export module lune:metal;

import :graphics_context;

namespace lune::metal
{
	constexpr int kMaxFramesInFlight = 3;

	struct TriangleData
	{
		vector_float3 positions[3];
	};

	TriangleData triangle = {{
		{0.0f, 0.5f, 0.0f},   // Top vertex
		{-0.5f, -0.5f, 0.0f}, // Bottom-left vertex
		{0.5f, -0.5f, 0.0f}   // Bottom-right vertex
	}};

	export class MetalContext final : public GraphicsContext
	{
		NS::SharedPtr<MTL::Device> m_device{};
		NS::SharedPtr<MTL::CommandQueue> m_commandQueue{};
		NS::SharedPtr<MTL::CommandBuffer> m_commandBuffer{};
		NS::SharedPtr<MTL::Library> m_library{};
		NS::SharedPtr<CA::MetalDrawable> m_drawable{};
		NS::SharedPtr<MTL::RenderPipelineState> m_pipelineState{};
		NS::SharedPtr<MTL::Buffer> m_triangleVertexBuffer{};
		NS::SharedPtr<MTL::RenderCommandEncoder> m_commandEncoder{};
		std::vector<NS::SharedPtr<CA::MetalLayer>> m_metalLayers;

	private:
		MetalContext();
		~MetalContext() override = default;

	public:
		MetalContext(const MetalContext&) = delete;
		MetalContext& operator=(const MetalContext&) = delete;

		static MetalContext& instance();

		[[nodiscard]] MTL::Device* device() const
		{
			return m_device.get();
		}

		[[nodiscard]] MTL::CommandQueue* commandQueue() const
		{
			return m_commandQueue.get();
		}

		[[nodiscard]] MTL::RenderPipelineState* renderPipelineState() const
		{
			return m_pipelineState.get();
		}

		[[nodiscard]] std::vector<NS::SharedPtr<CA::MetalLayer>> metalLayers() const
		{
			return m_metalLayers;
		}

		[[nodiscard]] MTL::RenderCommandEncoder* currentRenderEncoder() const
		{
			return m_commandEncoder.get();
		}

		void setCurrentEncoder(MTL::RenderCommandEncoder* encoder)
		{
			m_commandEncoder = NS::TransferPtr(encoder);
		}

		void createDevice();

		void createDefaultLibrary();
		void createLibrary(const std::string& path);
		void createCommandQueue();
		void createTriangle();
		void createRenderPipeline();

		void encodeRenderCommand(MTL::RenderCommandEncoder* renderEncoder) const;
		void sendRenderCommand();
		void draw();

		void loadShader(const std::string& name, const std::string& path,
		                bool isPrecompiled = false);
		MTL::Function* getFunction(const std::string& shaderName, const std::string& functionName);
		void reloadShaders();

		[[nodiscard]] NS::SharedPtr<CA::MetalLayer> createMetalLayer() const;

		void render() override;

		void addMetalLayer(const NS::SharedPtr<CA::MetalLayer>& metalLayer)
		{
			m_metalLayers.push_back(metalLayer);
		}

		void removeMetalLayer(const NS::SharedPtr<CA::MetalLayer>& metalLayer)
		{
			std::erase_if(
				m_metalLayers,
				[&](const NS::SharedPtr<CA::MetalLayer>& ptr)
				{
					return ptr.get() == metalLayer.get();
				}
				);
		}
	};
} // namespace lune::metal
