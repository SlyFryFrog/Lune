module;
#include <iostream>
module lune;

import vulkan_hpp;

namespace Lune
{
	vk::Bool32 debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
							 vk::DebugUtilsMessageTypeFlagsEXT type,
							 const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
	{
		std::cerr << "Validation layer: type " << to_string(type)
				  << " msg: " << pCallbackData->pMessage << std::endl;

		return vk::False;
	}
} // namespace Lune
