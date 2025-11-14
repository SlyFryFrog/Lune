module;
#include <objc/message.h>
#include <objc/runtime.h>
export module lune:objc_interop;

namespace lune
{
	export template <typename Ret, typename... Args>
	Ret objcCall(id obj, const char* selector, Args... args)
	{
		SEL sel = sel_registerName(selector);
		using FnType = Ret (*)(id, SEL, Args...);
		auto fn = reinterpret_cast<FnType>(objc_msgSend);
		return fn(obj, sel, args...);
	}
}
