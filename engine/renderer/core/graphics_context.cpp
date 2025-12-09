module;
module lune;

namespace lune::gfx
{
	GraphicsContext& GraphicsContext::instance()
	{
#ifdef USE_METAL
		static auto& s_instance = metal::MetalContext::instance();
#else
		static auto& s_instance = vulkan::VulkanContext::instance();
#endif
		return s_instance;
	}
} // namespace lune::gfx
