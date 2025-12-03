module;
#include <cstddef>
export module lune:graphics_context;

import :buffer;
import :texture;

namespace lune
{
	export class GraphicsContext
	{
	public:
		GraphicsContext() = default;
		virtual ~GraphicsContext() = default;

		GraphicsContext(const GraphicsContext&) = delete;
		GraphicsContext& operator=(const GraphicsContext&) = delete;

		static GraphicsContext& instance();

		[[nodiscard]] virtual Buffer createBuffer(size_t size) const = 0;
		[[nodiscard]] virtual Texture createTexture(
			const TextureContextCreateInfo& createInfo) const = 0;
	};
}
