module;
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include <simd/simd.h>
#include <AppKit/AppKit.hpp>
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
export module lune:metal;

import :graphics_context;

namespace lune::metal
{
	struct Vertex
	{
		simd::float4 position;
		simd::float4 color;
	};

	export class MetalContext final : public GraphicsContext
	{
		NS::SharedPtr<MTL::Device> m_device{nullptr};
		NS::SharedPtr<MTL::CommandQueue> m_commandQueue{nullptr};
		NS::SharedPtr<MTL::CommandBuffer> m_commandBuffer{nullptr};
		NS::SharedPtr<CA::MetalLayer> m_metalLayer{nullptr};
		NS::SharedPtr<MTL::Library> m_library{nullptr};
		NS::SharedPtr<MTL::RenderPipelineState> m_pipelineState{nullptr};
		NS::SharedPtr<MTL::Buffer> m_vertexBuffer{nullptr};

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
			return m_metalLayer.get();
		}

		void createDevice();

		CA::MetalLayer* createMetalLayer(double width, double height);

		void setupVertexBuffer();

		void setupPipeline();

		void draw();

		void render() override
		{
			draw();
		}
	};
}
