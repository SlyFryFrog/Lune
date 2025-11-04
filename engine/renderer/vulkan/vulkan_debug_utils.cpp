module;
#include <iostream>
module lune;

import vulkan_hpp;

namespace lune::vulkan
{
	vk::Bool32 debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
							 const vk::DebugUtilsMessageTypeFlagsEXT type,
							 const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
	{
		std::cerr << "Validation layer: type " << to_string(type)
				  << " msg: " << pCallbackData->pMessage << std::endl;

		return vk::False;
	}
}
