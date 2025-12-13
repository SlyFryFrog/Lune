module;
#include <QuartzCore/QuartzCore.hpp>
export module lune.metal:render_surface;

import lune.gfx;

namespace lune::metal
{
	export class MetalRenderSurfaceImpl : public gfx::IRenderSurfaceImpl
	{
		NS::SharedPtr<CA::MetalLayer> m_layer{};
		NS::SharedPtr<CA::MetalDrawable> m_drawable{};

	public:
		explicit MetalRenderSurfaceImpl(const gfx::RenderSurfaceInfo createInfo,
										MTL::Device* device) :
			IRenderSurfaceImpl(createInfo)
		{
			m_layer = NS::TransferPtr(CA::MetalLayer::layer());
			m_layer->setDevice(device);
			m_layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
			m_layer->setFramebufferOnly(false);
			m_layer->setDrawableSize(CGSizeMake(m_info.width, m_info.height));
		}

		[[nodiscard]] void* nextDrawable() override
		{
			const auto drawable{m_layer->nextDrawable()};
			if (!drawable) // Prevent crash
			{
				m_drawable.reset();
				return nullptr;
			}

			m_drawable = NS::TransferPtr(drawable);
			return m_drawable.get();
		}

		[[nodiscard]] void* currentDrawable() override
		{
			return m_drawable.get();
		}

		void setDrawableSize(const int width, const int height) const
		{
			m_layer->setDrawableSize(CGSizeMake(width, height));
		}

		[[nodiscard]] CA::MetalLayer* layer() const noexcept
		{
			return m_layer.get();
		}
	};

	export MetalRenderSurfaceImpl* toMetalImpl(const gfx::RenderSurface& surface)
	{
		auto* impl = gfx::getImpl(surface);

#ifndef NDEBUG
		auto* metalImpl = dynamic_cast<MetalRenderSurfaceImpl*>(impl);
		if (!metalImpl)
			throw std::runtime_error("RenderSurface is not a Metal surface!");
		return metalImpl;
#else
		return static_cast<MetalRenderSurfaceImpl*>(impl);
#endif
	}
} // namespace lune::metal
