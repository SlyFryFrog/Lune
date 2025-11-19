module;
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <vector>
#include <span>
export module lune:metal_context;

import :graphics_context;
import :metal_shader;

namespace lune::metal
{
	export struct MetalContextCreateInfo
	{
		MTL::ClearColor clearColor = {0.0f, 0.0f, 0.0f, 0.0f};
		MTL::LoadAction loadAction = MTL::LoadActionClear;
		MTL::StoreAction storeAction = MTL::StoreActionStore;
		int maxFramesInFlight = 3;
	};


	export class MetalContext final : public GraphicsContext
	{
		NS::SharedPtr<MTL::Device> m_device{};
		NS::SharedPtr<MTL::CommandQueue> m_commandQueue{};
		std::vector<NS::SharedPtr<CA::MetalLayer>> m_metalLayers{};
		std::vector<std::shared_ptr<GraphicsShader>> m_graphicsShaders{};
		MetalContextCreateInfo m_createInfo{};

	private:
		MetalContext();
		~MetalContext() override = default;

	public:
		MetalContext(const MetalContext&) = delete;
		MetalContext& operator=(const MetalContext&) = delete;

		/**
		 * @return The singleton for the Metal renderer.
		 */
		static MetalContext& instance();
		void create(const MetalContextCreateInfo& info);

		void createDefaultDevice();
		void createCommandQueue();

		void sendRenderCommands(const NS::SharedPtr<CA::MetalDrawable>& drawable) const;
		void draw() override;

		void addMetalLayer(const NS::SharedPtr<CA::MetalLayer>& metalLayer);
		void removeMetalLayer(const NS::SharedPtr<CA::MetalLayer>& metalLayer);

		void addShader(const std::shared_ptr<GraphicsShader>& metalShader);

		[[nodiscard]] MTL::Device* device() const
		{
			return m_device.get();
		}

		[[nodiscard]] MTL::CommandQueue* commandQueue() const
		{
			return m_commandQueue.get();
		}

		[[nodiscard]] std::span<const NS::SharedPtr<CA::MetalLayer>> metalLayers() const
		{
			return m_metalLayers;
		}

		[[nodiscard]] MetalContextCreateInfo& createInfo()
		{
			return m_createInfo;
		}
	};
}
