module;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
export module lune:window;

import vulkan_hpp;
import :input_manager;

namespace lune
{
	export struct WindowCreateInfo
	{
		int width = 800;
		int height = 600;
		std::string title = "Window";
		bool resizable = true;
	};


	export class Window
	{
		GLFWwindow* m_handle = nullptr;
		int m_width = 0;
		int m_height = 0;
		std::string m_title;

	public:
		explicit Window() = default;
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		// Movable
		Window(Window&& other) noexcept;
		Window& operator=(Window&& other) noexcept;

		[[nodiscard]] GLFWwindow* handle() const
		{
			return m_handle;
		}

		void show() const;
		void resize(int width, int height) const;
		[[nodiscard]] bool shouldClose() const;
		void setShouldClose(bool shouldClose) const;

		static void pollEvents();

		void create(const WindowCreateInfo& info);
		void destroy();
	};


	namespace raii
	{
		export class Window
		{
			lune::Window m_rawWindow;

		public:
			explicit Window(const WindowCreateInfo& createInfo)
			{
				m_rawWindow.create(createInfo);
			}

			~Window()
			{
				if (m_rawWindow.handle())
					m_rawWindow.destroy();
			}

			Window(const Window&) = delete;
			Window& operator=(const Window&) = delete;

			Window(Window&& other) noexcept;
			Window& operator=(Window&& other) noexcept;

			[[nodiscard]] GLFWwindow* handle() const
			{
				return m_rawWindow.handle();
			}

			void show() const
			{
				m_rawWindow.show();
			}

			void resize(const int width, const int height) const
			{
				m_rawWindow.resize(width, height);
			}

			[[nodiscard]] bool shouldClose() const
			{
				return m_rawWindow.shouldClose();
			}

			void setShouldClose(const bool shouldClose) const
			{
				m_rawWindow.setShouldClose(shouldClose);
			}
		};
	} // namespace raii
} // namespace lune
