module;
module lune.gfx;

import lune.metal;

namespace lune::gfx
{
	GraphicsContext& GraphicsContext::instance()
	{
		static GraphicsContext* s_instance;
#ifdef USE_METAL
		s_instance = &metal::MetalContext::instance();
#elif defined(USE_VULKAN)
		s_instance = &vulkan::VulkanContext::instance();
#else
#error Unsupported backend.
#endif
		return *s_instance;
	}
} // namespace lune::gfx
