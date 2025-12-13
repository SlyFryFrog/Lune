module;
#include <Metal/Metal.hpp>
#include <memory>
export module lune.metal:context;

import :buffer;
import :texture;
import :render_surface;
import :graphics;
import lune.gfx;

namespace lune::metal
{
	export class MetalContext final : public gfx::Context
	{
		NS::SharedPtr<MTL::Device> m_device{};
		NS::SharedPtr<MTL::CommandQueue> m_commandQueue{};

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

		[[nodiscard]] gfx::Shader createShader(gfx::ShaderDesc desc) const override
		{
			auto impl{std::make_unique<MetalShaderImpl>(m_device.get(), desc)};
			return gfx::Shader(std::move(impl));
		}

		[[nodiscard]] gfx::Pipeline createPipeline(const gfx::Shader& shader,
												   gfx::PipelineDesc desc) const override
		{
			auto impl = std::make_unique<MetalPipelineImpl>(shader, desc);
			return gfx::Pipeline(std::move(impl));
		}

		[[nodiscard]] gfx::Material createMaterial(const gfx::Pipeline& pipeline) const override
		{
			auto impl{std::make_unique<MetalMaterialImpl>(pipeline)};
			return gfx::Material(std::move(impl));
		}

		[[nodiscard]] gfx::RenderPass
		createRenderPass(const gfx::RenderSurface& surface) const override
		{
			auto impl{std::make_unique<MetalRenderPassImpl>(surface)};
			return gfx::RenderPass(std::move(impl));
		}
	};
} // namespace lune::metal
