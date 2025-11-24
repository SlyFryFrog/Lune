module;
export module lune:graphics_context;

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
	};
}
