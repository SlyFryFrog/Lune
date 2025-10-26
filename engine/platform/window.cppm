module;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
export module lune:window;

import vulkan_hpp;

namespace lune
{
	class Window
	{
		GLFWwindow* m_window{nullptr};
		std::string m_title{"Untitled Window"};
		uint32_t m_width{800};
		uint32_t m_height{600};

	public:
		Window() = default;

		~Window()
		{
			cleanup();
		}

		void init();

		bool isDone() const
		{
			return glfwWindowShouldClose(m_window);
		}

		void setIsDone(const bool value) const
		{
			glfwSetWindowShouldClose(m_window, value);
		}

		void pollEvents()
		{
			glfwPollEvents();
		}

		void cleanup();

		GLFWwindow* getNativeWindow() const
		{
			return m_window;
		}

		uint32_t getWidth() const
		{
			return m_width;
		}
		uint32_t getHeight() const
		{
			return m_height;
		}
	};
} // namespace lune
