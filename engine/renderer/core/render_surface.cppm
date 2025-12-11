module;
#include <memory>
export module lune.gfx:render_surface;

namespace lune::gfx
{
	/**
	 * @brief Information about the rendering surface.
	 */
	export struct RenderSurfaceInfo
	{
		int width{0};
		int height{0};
		bool vsync{true};
	};

	/**
	 * @brief Backend interface for a rendering surface.
	 */
	export class IRenderSurfaceImpl
	{
	protected:
		RenderSurfaceInfo m_info;

	public:
		explicit IRenderSurfaceImpl(const RenderSurfaceInfo& info) : m_info(info)
		{
		}

		virtual ~IRenderSurfaceImpl() = default;

		virtual void* currentDrawable() = 0;
		virtual void* nextDrawable() = 0;

		[[nodiscard]] const RenderSurfaceInfo& info() const noexcept
		{
			return m_info;
		}
	};

	/**
	 * @brief Frontend wrapper for a rendering surface.
	 */
	export class RenderSurface
	{
		std::unique_ptr<IRenderSurfaceImpl> m_impl;
		friend IRenderSurfaceImpl* getImpl(const RenderSurface& surface);

	public:
		explicit RenderSurface(std::unique_ptr<IRenderSurfaceImpl> impl) : m_impl(std::move(impl))
		{
		}

		[[nodiscard]] void* currentDrawable() const
		{
			return m_impl->currentDrawable();
		}

		[[nodiscard]] void* nextDrawable() const
		{
			return m_impl->nextDrawable();
		}

		[[nodiscard]] const RenderSurfaceInfo& info() const noexcept
		{
			return m_impl->info();
		}
	};

	export IRenderSurfaceImpl* getImpl(const RenderSurface& surface)
	{
		return surface.m_impl.get();
	}
} // namespace lune::gfx
