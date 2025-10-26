module;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
export module lune:window;

import vulkan_hpp;

namespace Lune
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

		bool is_done() const
		{
			return glfwWindowShouldClose(m_window);
		}

		void set_is_done(const bool value) const
		{
			glfwSetWindowShouldClose(m_window, value);
		}

		void poll_events()
		{
			glfwPollEvents();
		}

		void cleanup();

		GLFWwindow* get_native_window() const
		{
			return m_window;
		}

		uint32_t get_width() const
		{
			return m_width;
		}
		uint32_t get_height() const
		{
			return m_height;
		}
	};
} // namespace Lune
