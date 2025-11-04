module;
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
module lune;

import vulkan_hpp;
import :vulkan_debug_utils;

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

namespace lune::vulkan
{
	VulkanContext& VulkanContext::instance()
	{
		static VulkanContext s_instance;
		return s_instance;
	}

	void render() override
	{
	}

	void VulkanContext::init()
	{
		createInstance();
		setupDebugMessenger();
		pickPhysicalDevice();
	}

	void VulkanContext::createInstance()
	{
		// Check if validation layers are requested and supported
		if (enableValidationLayers && !checkValidationLayerSupport())
		{
			throw std::runtime_error("Validation layers requested but not available!");
		}

		// Application info
		constexpr vk::ApplicationInfo appInfo{.pApplicationName = "Hello Triangle",
		                                      .applicationVersion = vk::makeVersion(1, 0, 0),
		                                      .pEngineName = "No Engine",
		                                      .engineVersion = vk::makeVersion(1, 0, 0),
		                                      .apiVersion = vk::ApiVersion14};

		// Get required extensions (GLFW + validation layers if enabled)
		auto extensions = getRequiredExtensions();

		// Check if all required extensions are supported
		auto extensionProperties = vk::enumerateInstanceExtensionProperties();
		for (const auto& extension : extensions)
		{
			if (std::ranges::none_of(extensionProperties,
			                         [extension](const vk::ExtensionProperties& prop)
			                         {
				                         return strcmp(prop.extensionName, extension) == 0;
			                         }))
			{
				throw std::runtime_error(std::string("Required Vulkan extension not supported: ") +
					extension);
			}
		}

		// Instance creation
		vk::InstanceCreateInfo createInfo{
			.flags = vk::InstanceCreateFlags(),
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = static_cast<uint32_t>(
				enableValidationLayers ? m_validationLayers.size() : 0u),
			.ppEnabledLayerNames = enableValidationLayers ? m_validationLayers.data() : nullptr,
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data()};

#ifdef __APPLE__
		createInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

		// Create the Vulkan instance
		m_instance = vk::raii::Instance(m_context, createInfo);
	}

	void VulkanContext::setupDebugMessenger()
	{
		if constexpr (!enableValidationLayers)
		{
			return;
		}

		constexpr vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{
			.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			.pfnUserCallback = &debugCallback,
		};

		m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(debugCreateInfo);
	}

	bool VulkanContext::checkValidationLayerSupport() const
	{
		auto layerProperties = vk::enumerateInstanceLayerProperties();
		for (const auto& layerName : m_validationLayers)
		{
			if (std::ranges::none_of(layerProperties, [layerName](const vk::LayerProperties& prop)
			{
				return strcmp(prop.layerName, layerName) == 0;
			}))
			{
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> VulkanContext::getRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers)
		{
			extensions.push_back(vk::EXTDebugUtilsExtensionName);
		}

		return extensions;
	}

	void VulkanContext::pickPhysicalDevice()
	{
		const auto devices = m_instance.enumeratePhysicalDevices();
		if (devices.empty())
		{
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");
		}

		std::multimap<int, vk::raii::PhysicalDevice> candidates;
		for (const auto& device : devices)
		{
			const auto deviceProperties = device.getProperties();
			const auto deviceFeatures = device.getFeatures();
			uint32_t score = 0;

			// Discrete GPUs have a significant performance advantage
			if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			{
				score += 1000;
			}

			// Maximum possible size of textures affects graphics quality
			score += deviceProperties.limits.maxImageDimension2D;

			// Application can't function without geometry shaders
			if (!deviceFeatures.geometryShader)
			{
				continue;
			}
			candidates.insert(std::make_pair(score, device));
		}

		if (candidates.rbegin()->first > 0)
		{
			m_physicalDevice = std::make_unique<vk::raii::PhysicalDevice>(
				candidates.rbegin()->second);
		}
		else
		{
			throw std::runtime_error("Failed to find a suitable GPU!");
		}
	}

	void VulkanContext::createLogicalDevice()
	{
		if (!m_physicalDevice)
			throw std::runtime_error("Physical device not selected!");

		std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
			m_physicalDevice->getQueueFamilyProperties();

		uint32_t graphicsFamilyIndex = findQueueFamilies(*m_physicalDevice);
		float queuePriority = 0.0f;
		vk::DeviceQueueCreateInfo deviceQueueCreateInfo{.queueFamilyIndex = graphicsFamilyIndex,
		                                                .queueCount = 1,
		                                                .pQueuePriorities = &queuePriority};

		vk::PhysicalDeviceFeatures deviceFeatures;
		// Create a chain of feature structures
		vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features,
		                   vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
			featureChain = {
				{},                            // vk::PhysicalDeviceFeatures2 (empty for now)
				{.dynamicRendering = true},    // Enable dynamic rendering from Vulkan 1.3
				{.extendedDynamicState = true} // Enable extended dynamic state from the extension
			};

		std::vector<const char*> deviceExtensions = {
			vk::KHRSwapchainExtensionName,
			vk::KHRSpirv14ExtensionName,
			vk::KHRSynchronization2ExtensionName,
			vk::KHRCreateRenderpass2ExtensionName
		};

		vk::DeviceCreateInfo deviceCreateInfo{
			.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &deviceQueueCreateInfo,
			.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
			.ppEnabledExtensionNames = deviceExtensions.data()
		};

		m_device = vk::raii::Device(*m_physicalDevice, deviceCreateInfo);
		m_queue = vk::raii::Queue(m_device, graphicsFamilyIndex, 0);
	}

	uint32_t VulkanContext::findQueueFamilies(const vk::raii::PhysicalDevice& device) const
	{
		// find the index of the first queue family that supports graphics
		std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
			m_physicalDevice->getQueueFamilyProperties();

		// get the first index into queueFamilyProperties which supports graphics
		auto graphicsQueueFamilyProperty = std::find_if(
			queueFamilyProperties.begin(), queueFamilyProperties.end(),
			[](vk::QueueFamilyProperties const& qfp)
			{
				return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
			});

		return static_cast<uint32_t>(
			std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
	}
}
