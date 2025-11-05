module;
#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <AppKit/AppKit.hpp>

#include <iostream>
#include <stdexcept>
#include <string>
module lune;

import :input_manager;
import :metal;

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

	void Window::attachMetalToGLFW()
	{
		auto nswindow = reinterpret_cast<NS::Window*>(glfwGetCocoaWindow(m_handle));
		auto device = lune::metal::MetalContext::instance().device();
		auto commandQueue = device->newCommandQueue();
		auto layer = CA::MetalLayer::layer();
		layer->setDevice(device);

		auto nsview = nswindow->contentView();
		nsview->setLayer(layer);
		nsview->setWantsLayer(true);
		nsview->setOpaque(true);

		auto color = MTL::ClearColor::Make(0, 1, 0, 1);
		while (!glfwWindowShouldClose(m_handle))
		{
			lune::Window::pollEvents();

			auto autoReleasePool = NS::AutoreleasePool::alloc()->init();

			color.red = color.red > 1.0 ? 0.0 : color.red + 0.01;

			auto surface = layer->nextDrawable();
			auto pass = MTL::RenderPassDescriptor::renderPassDescriptor();
			auto passColorAttachment0 = pass->colorAttachments()->object(0);
			passColorAttachment0->setClearColor(color);
			passColorAttachment0->setLoadAction(MTL::LoadActionClear);
			passColorAttachment0->setStoreAction(MTL::StoreActionStore);
			passColorAttachment0->setTexture(surface->texture());

			auto commandBuffer = commandQueue->commandBuffer();
			auto encoder = commandBuffer->renderCommandEncoder(pass);
			encoder->endEncoding();
			commandBuffer->presentDrawable(surface);
			commandBuffer->commit();

			autoReleasePool->release();
		}
	}
} // namespace lune
