module;
#include <vector>
export module lune:vulkan_context;

import vulkan_hpp;

namespace lune
{
	export class VulkanContext
	{
		vk::raii::Context m_context;
		vk::raii::Instance m_instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
		vk::raii::PhysicalDevice m_physicalDevice = nullptr;
		const std::vector<const char*> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};

	public:
		void init();
		void createInstance();

		[[nodiscard]] const vk::raii::Instance& getInstance() const
		{
			return m_instance;
		}

	private:
		void setupDebugMessenger();
		[[nodiscard]] bool checkValidationLayerSupport() const;
		[[nodiscard]] static std::vector<const char*> getRequiredExtensions();
		void pickPhysicalDevice();
	};
} // namespace lune
