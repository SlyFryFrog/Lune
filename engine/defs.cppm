module;
export module lune:defs;

namespace lune
{
#if defined(NDEBUG)
	constexpr bool kNoExcept = true;
#else
	constexpr bool kNoExcept = false;
#endif
}
