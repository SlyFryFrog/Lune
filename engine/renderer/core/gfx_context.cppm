module;
#include <string>
export module lune.gfx:context;

import :buffer;
import :texture;
import :render_surface;
import :graphics;
import :compute;

namespace lune::gfx
{
	export class Context
	{
	public:
		Context() = default;
		virtual ~Context() = default;

		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;

		static Context& instance();

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
} // namespace lune::gfx
