module;
module lune;

namespace lune
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
}
