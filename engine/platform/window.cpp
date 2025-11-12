module;
#ifdef USE_METAL
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#include <QuartzCore/QuartzCore.hpp>
#endif

#include <iostream>
#include <stdexcept>
#include <string>
module lune;

#ifdef USE_METAL
import :metal_context;
import :objc_interop;
#endif


import :input_manager;

namespace lune
{
	static bool glfwInitialized = false;

	Window::~Window()
	{
		destroy();
	}

	void Window::create(const WindowCreateInfo& info)
	{
		if (!glfwInitialized)
		{
			if (!glfwInit())
				throw std::runtime_error("Failed to initialize GLFW!");
			glfwInitialized = true;
		}

		destroy();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, info.resizable ? GLFW_TRUE : GLFW_FALSE);

		m_handle = glfwCreateWindow(info.width, info.height, info.title.c_str(), nullptr, nullptr);
		if (!m_handle)
			throw std::runtime_error("Failed to create GLFW window!");

		m_width = info.width;
		m_height = info.height;
		m_title = info.title;
		m_mode = info.mode;

		// Set callbacks
		glfwSetWindowUserPointer(m_handle, this);
		glfwSetKeyCallback(m_handle, InputManager::_processInputCallback);
		glfwSetCursorPosCallback(m_handle, InputManager::_processMouseCallback);
		glfwSetMouseButtonCallback(m_handle, InputManager::_processMouseButtonCallback);
		glfwSetFramebufferSizeCallback(m_handle, _onFrameBufferSizeCallback);

#ifdef USE_METAL
		attachMetalToGLFW();
#endif
	}

	void Window::destroy()
	{
		if (m_handle)
		{
			glfwDestroyWindow(m_handle);
			m_handle = nullptr;
		}
	}

	void Window::_onFrameBufferSizeCallback(GLFWwindow* handle, const int width, const int height)
	{
		const auto window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
		window->m_metalLayer->setDrawableSize(CGSizeMake(width, height));
	}

	Window::Window(Window&& other) noexcept :
		m_handle(other.m_handle),
		m_width(other.m_width),
		m_height(other.m_height),
		m_title(std::move(other.m_title))
	{
		other.m_handle = nullptr;
	}

	Window& Window::operator=(Window&& other) noexcept
	{
		if (this != &other)
		{
			destroy();
			m_handle = other.m_handle;
			m_width = other.m_width;
			m_height = other.m_height;
			m_title = std::move(other.m_title);
			other.m_handle = nullptr;
		}
		return *this;
	}

	void Window::show() const
	{
		if (m_handle)
		{
			glfwShowWindow(m_handle);
		}
	}

	void Window::resize(const int width, const int height) const
	{
		if (m_handle)
		{
			glfwSetWindowSize(m_handle, width, height);
			std::cout << "Resized window \"" << m_title << "\" to " << width << "x" << height
				<< "\n";
		}
	}

	bool Window::shouldClose() const
	{
		return m_handle && glfwWindowShouldClose(m_handle);
	}

	void Window::setShouldClose(const bool shouldClose) const
	{
		glfwSetWindowShouldClose(m_handle, shouldClose);
	}

	void Window::setTitle(const std::string& title)
	{
		m_title = title;
		glfwSetWindowTitle(m_handle, title.c_str());
	}

	std::string Window::getTitle() const
	{
		return m_title;
	}

	void Window::setResizable(const bool resizable)
	{
		glfwWindowHint(GLFW_RESIZABLE, resizable);
	}

	void Window::setWindowMode(WindowMode mode)
	{
		// TODO
	}

	WindowMode Window::getWindowMode() const
	{
		return m_mode;
	}

	void Window::pollEvents()
	{
		glfwPollEvents();
	}

#ifdef USE_METAL
	void Window::attachMetalToGLFW()
	{
		const id nsWindow = glfwGetCocoaWindow(m_handle);
		const id nsView = objcCall<id>(nsWindow, "contentView");

		auto& metalCtx = metal::MetalContext::instance();
		m_metalLayer = NS::TransferPtr(CA::MetalLayer::layer());
		m_metalLayer->setDevice(metalCtx.device());
		m_metalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
		m_metalLayer->setFramebufferOnly(false);
		m_metalLayer->setDrawableSize(CGSizeMake(m_width, m_height));

		metalCtx.addMetalLayer(m_metalLayer);

		objcCall<void>(nsView, "setLayer:", m_metalLayer.get());
	}
#endif
}
