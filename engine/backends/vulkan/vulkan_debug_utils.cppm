module;
export module lune:vulkan_debug_utils;

import vulkan_hpp;

namespace lune::vulkan
{
	export vk::Bool32 debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
									vk::DebugUtilsMessageTypeFlagsEXT type,
									const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
									void*);
}
