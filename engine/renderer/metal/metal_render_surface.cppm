module;
#include <QuartzCore/QuartzCore.hpp>
export module lune:metal_render_surface;

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
			auto* drawable = m_layer->nextDrawable();
			if (!drawable)	// Prevent crash
			{
				m_drawable.reset();
				return nullptr;
			}

			m_drawable = NS::TransferPtr(drawable);
			return m_drawable.get();
		}

		[[nodiscard]] CA::MetalDrawable* drawable() const noexcept
		{
			return m_drawable.get();
		}

		[[nodiscard]] CA::MetalLayer* layer() const noexcept
		{
			return m_layer.get();
		}
	};
}
