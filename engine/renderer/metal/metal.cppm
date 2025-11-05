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
	export class MetalLayer
	{
	public:
		virtual ~MetalLayer() = default;
		virtual void setup() = 0;

		virtual void render() = 0;
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
		NS::SharedPtr<MTL::RenderCommandEncoder> m_currentEncoder{nullptr};

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
			return m_metalLayer.get();
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
			return m_currentEncoder.get();
		}

		void setCurrentEncoder(MTL::RenderCommandEncoder* encoder)
		{
			m_currentEncoder = NS::TransferPtr(encoder);
		}

		void createDevice();

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
}
