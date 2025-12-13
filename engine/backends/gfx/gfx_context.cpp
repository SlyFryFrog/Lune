module;
#include <memory>
module lune.gfx;

#ifdef USE_METAL
import lune.metal;
#endif

namespace lune::gfx
{
	Context::Context()
	{

#ifdef USE_METAL
		m_impl = std::make_unique<metal::MetalContextImpl>();
#elif defined(USE_VULKAN)
#endif
	}
} // namespace lune::gfx
