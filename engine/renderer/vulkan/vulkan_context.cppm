module;
#include <vector>
export module lune:vulkan_context;

import vulkan_hpp;
import :graphics_context;

namespace lune::vulkan
{
	export class VulkanContext : public GraphicsContext
	{
		vk::raii::Context m_context;
		vk::raii::Instance m_instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
		vk::raii::Device m_device = nullptr;
		vk::raii::Queue m_queue = nullptr;
		vk::raii::SurfaceKHR m_surface = nullptr;
		std::unique_ptr<vk::raii::PhysicalDevice> m_physicalDevice = nullptr;
		const std::vector<const char*> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};

	public:
		static VulkanContext& instance();

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
		void createLogicalDevice();
		[[nodiscard]] uint32_t findQueueFamilies(const vk::raii::PhysicalDevice& device) const;
	};
}
