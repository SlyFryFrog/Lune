module;
#include <memory>
#include <string>
export module lune.gfx:context;

import :buffer;
import :texture;
import :render_surface;
import :graphics;
import :compute;

namespace lune::gfx
{
	export class IContextImpl
	{
	public:
		IContextImpl() = default;
		virtual ~IContextImpl() = default;

		[[nodiscard]] virtual Buffer createBuffer(size_t size) const = 0;
		[[nodiscard]] virtual Texture
		createTexture(const TextureContextCreateInfo& createInfo) const = 0;

		[[nodiscard]] virtual Shader createShader(ShaderDesc desc) const = 0;
		[[nodiscard]] virtual Pipeline createPipeline(const Shader& shader,
													  PipelineDesc desc) const = 0;
		[[nodiscard]] virtual Material createMaterial(const Pipeline& pipeline) const = 0;
		[[nodiscard]] virtual RenderPass createRenderPass(const RenderSurface& surface) const = 0;

		[[nodiscard]] virtual ComputeShader createComputeShader(const std::string& path) const = 0;
	};

	export class Context
	{
		std::unique_ptr<IContextImpl> m_impl;

	public:
		Context();

		explicit Context(std::unique_ptr<IContextImpl> impl) : m_impl(std::move(impl))
		{
		}

		~Context() = default;

		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;

		[[nodiscard]] IContextImpl* getImpl() const noexcept
		{
			return m_impl.get();
		}

		[[nodiscard]] Buffer createBuffer(size_t size) const
		{
			return m_impl->createBuffer(size);
		}

		[[nodiscard]] Texture createTexture(const TextureContextCreateInfo& createInfo) const
		{
			return m_impl->createTexture(createInfo);
		}

		[[nodiscard]] Shader createShader(const ShaderDesc& desc) const
		{
			return m_impl->createShader(desc);
		}

		[[nodiscard]] Pipeline createPipeline(const Shader& shader, const PipelineDesc& desc) const
		{
			return m_impl->createPipeline(shader, desc);
		}

		[[nodiscard]] Material createMaterial(const Pipeline& pipeline) const
		{
			return m_impl->createMaterial(pipeline);
		}

		[[nodiscard]] RenderPass createRenderPass(const RenderSurface& surface) const
		{
			return m_impl->createRenderPass(surface);
		}

		[[nodiscard]] ComputeShader createComputeShader(const std::string& path) const
		{
			return m_impl->createComputeShader(path);
		}
	};
} // namespace lune::gfx
