module;
#ifdef USE_METAL
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#include <AppKit/AppKit.hpp>
#endif

#include <iostream>
#include <stdexcept>
#include <string>
module lune;

#ifdef USE_METAL
import :metal;
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
		glfwSetKeyCallback(m_handle, InputManager::_processInputCallback);
		glfwSetCursorPosCallback(m_handle, InputManager::_processMouseCallback);
		glfwSetMouseButtonCallback(m_handle, InputManager::_processMouseButtonCallback);
	}

	void Window::destroy()
	{
		if (m_handle)
		{
			glfwDestroyWindow(m_handle);
			m_handle = nullptr;
		}
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

	WindowMode Window::getWindowMode() const
	{
		return m_mode;
	}

	void Window::pollEvents()
	{
		glfwPollEvents();
	}

	void Window::attachMetalToGLFW() const
	{
#ifdef USE_METAL
		NS::Window* nswindow = reinterpret_cast<NS::Window*>(glfwGetCocoaWindow(m_handle));
		NS::View* nsview = nswindow->contentView();
		nsview->setLayer(metal::MetalContext::instance().createMetalLayer(m_width, m_height));
		nsview->setWantsLayer(true);
		nsview->setOpaque(true);
#else
		std::cout << "Can't attach GLFW window to Metal, we're not using Metal!\n";
#endif
	}
} // namespace lune
