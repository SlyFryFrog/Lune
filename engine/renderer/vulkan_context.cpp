module;
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
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

namespace lune
{
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
									 { return strcmp(prop.extensionName, extension) == 0; }))
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
		if (!enableValidationLayers)
		{
			return;
		}

		vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{
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
									 { return strcmp(prop.layerName, layerName) == 0; }))
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

	}
} // namespace Lune
