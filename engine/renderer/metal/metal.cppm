module;
#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
export module lune:metal;

import :graphics_context;

namespace lune::metal
{
	export class MetalContext : public GraphicsContext
	{
		MTL::Device* m_device{nullptr};
		MTL::CommandQueue* m_commandQueue{nullptr};
		CA::MetalLayer* m_metalLayer{nullptr};

	private:
		MetalContext();
		~MetalContext();

	public:
		MetalContext(const MetalContext&) = delete;
		MetalContext& operator=(const MetalContext&) = delete;

		static MetalContext& instance();

		[[nodiscard]] MTL::Device* device() const
		{
			return m_device;
		}

		[[nodiscard]] MTL::CommandQueue* commandQueue() const
		{
			return m_commandQueue;
		}

		[[nodiscard]] CA::MetalLayer* metalLayer() const
		{
			return m_metalLayer;
		}

		void render() override;
	};
}
