module;
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <vector>
#include <memory>
export module lune:metal_context;

import :graphics_context;
import :metal_mappings;
import :metal_buffer;
import :texture;
import :metal_texture;

namespace lune::metal
{
	export class RenderSurface
	{
		NS::SharedPtr<CA::MetalLayer> m_layer{};
		NS::SharedPtr<CA::MetalDrawable> m_drawable{};

	public:
		explicit RenderSurface(const NS::SharedPtr<CA::MetalLayer>& layer) :
			m_layer(layer)
		{
		}

		[[nodiscard]] CA::MetalDrawable* nextDrawable()
		{
			m_drawable = NS::TransferPtr(m_layer->nextDrawable());
			return m_drawable.get();
		}

		[[nodiscard]] CA::MetalDrawable* drawable() const
		{
			return m_drawable.get();
		}

		[[nodiscard]] CA::MetalLayer* layer() const
		{
			return m_layer.get();
		}
	};


	export class MetalContext final : public GraphicsContext
	{
		NS::SharedPtr<MTL::Device> m_device{};
		NS::SharedPtr<MTL::CommandQueue> m_commandQueue{};
		std::vector<std::shared_ptr<RenderSurface>> m_surfaces{};

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

		void createDefaultDevice();
		void createCommandQueue();

		[[nodiscard]] MTL::Device* device() const
		{
			return m_device.get();
		}

		[[nodiscard]] MTL::CommandQueue* commandQueue() const
		{
			return m_commandQueue.get();
		}

		void addSurface(const std::shared_ptr<RenderSurface>& surface)
		{
			m_surfaces.push_back(surface);
		}

		void removeSurface(const std::shared_ptr<RenderSurface>& surface)
		{
			std::erase(m_surfaces, surface);
		}

		[[nodiscard]] std::vector<std::shared_ptr<RenderSurface>> surfaces() const
		{
			return m_surfaces;
		}

		[[nodiscard]] Buffer createBuffer(size_t size) const
		{
			auto impl = std::make_unique<MetalBufferImpl>(m_device.get(), size);
			return Buffer(std::move(impl));
		}

		[[nodiscard]] Texture createTexture(TextureContextCreateInfo createInfo) const
		{
			auto impl = std::make_unique<MetalTextureImpl>(m_device.get(), createInfo);
			return Texture(std::move(impl));
		}
	};
}
