module;
#include <Metal/Metal.hpp>
#include <memory>
#include <vector>
export module lune:metal_context;

import :graphics_context;
import :texture;
import :metal_buffer;
import :metal_texture;
import :metal_mappings;
import :metal_render_surface;

namespace lune::metal
{
	export class MetalContext final : public gfx::GraphicsContext
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

		[[nodiscard]] gfx::Buffer createBuffer(const size_t size) const override
		{
			auto impl{std::make_unique<MetalBufferImpl>(m_device.get(), size)};
			return gfx::Buffer(std::move(impl));
		}

		[[nodiscard]] gfx::Texture
		createTexture(const gfx::TextureContextCreateInfo& createInfo) const override
		{
			auto impl{std::make_unique<MetalTextureImpl>(m_device.get(), createInfo)};
			return gfx::Texture(std::move(impl));
		}
	};
} // namespace lune::metal
