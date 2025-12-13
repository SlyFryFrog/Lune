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

	public:
		explicit RenderSurface(std::unique_ptr<IRenderSurfaceImpl> impl) : m_impl(std::move(impl))
		{
		}

		[[nodiscard]] IRenderSurfaceImpl* getImpl() const
		{
			return m_impl.get();
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
} // namespace lune::gfx
