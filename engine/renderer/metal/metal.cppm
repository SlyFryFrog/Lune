module;
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include <AppKit/AppKit.hpp>
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <simd/vector_types.h>
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
		{0.0f, 0.5f, 0.0f},	  // Top vertex
		{-0.5f, -0.5f, 0.0f}, // Bottom-left vertex
		{0.5f, -0.5f, 0.0f}	  // Bottom-right vertex
	}};

	export class MetalLayer
	{
	public:
		virtual ~MetalLayer() = default;
		virtual void setup() = 0;

		virtual void render() = 0;
	};

	export class MetalContext final : public GraphicsContext
	{
		NS::SharedPtr<MTL::Device> m_device{};
		NS::SharedPtr<MTL::CommandQueue> m_commandQueue{};
		NS::SharedPtr<MTL::CommandBuffer> m_commandBuffer{};
		NS::SharedPtr<MTL::Library> m_library{};
		NS::SharedPtr<CA::MetalDrawable> m_drawable{};
		NS::SharedPtr<CA::MetalLayer> m_layer{};
		NS::SharedPtr<MTL::RenderPipelineState> m_pipelineState{};
		NS::SharedPtr<MTL::Buffer> m_triangleVertexBuffer{};
		NS::SharedPtr<MTL::RenderCommandEncoder> m_commandEncoder{};

		std::vector<std::shared_ptr<MetalLayer>> m_layers;

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

		[[nodiscard]] CA::MetalLayer* metalLayer() const
		{
			return m_layer.get();
		}

		[[nodiscard]] MTL::RenderPipelineState* renderPipelineState() const
		{
			return m_pipelineState.get();
		}

		[[nodiscard]] std::vector<std::shared_ptr<MetalLayer>> layers() const
		{
			return m_layers;
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
		void createLibrary();
		void createCommandQueue();
		void createTriangle();
		void createRenderPipeline();

		void encodeRenderCommand(MTL::RenderCommandEncoder* renderEncoder) const;
		void sendRenderCommand();
		void draw();

		CA::MetalLayer* createMetalLayer(double width, double height);

		void render() override;

		template <typename T, typename... Args>
		T& addLayer(Args&&... args)
		{
			auto layer = std::make_shared<T>(std::forward<Args>(args)...);
			T& ref = *layer;
			m_layers.push_back(std::move(layer));
			ref.setup();
			return ref;
		}

		void removeLayer(const size_t index)
		{
			if (index < m_layers.size())
				m_layers.erase(m_layers.begin() + index);
		}
	};

	export class MetalDemo
	{
		MetalDemo();

		~MetalDemo();

	public:
		MTL::Device* m_device{};
		MTL::CommandQueue* m_commandQueue{};
		MTL::CommandBuffer* m_commandBuffer{};
		MTL::Library* m_library{};
		CA::MetalDrawable* m_drawable{};
		CA::MetalLayer* m_layer{};
		MTL::RenderPipelineState* m_renderPSO{};
		MTL::Buffer* m_triangleVertexBuffer{};

		static MetalDemo& instance()
		{
			static MetalDemo s_instance;
			return s_instance;
		}

		void createDevice();
		void createDefaultLibrary();
		void createCommandQueue();
		void createTriangle();
		void createRenderPipeline();

		void encodeRenderCommand(MTL::RenderCommandEncoder* renderEncoder) const;
		void sendRenderCommand();
		void draw();
	};
} // namespace lune::metal
