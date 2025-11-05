module;
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
export module lune:window;

import :input_manager;

namespace lune
{
	export enum WindowMode
	{
		FULLSCREEN,
		WINDOWED,
		WINDOWED_FULLSCREEN,
	};


	export struct WindowCreateInfo
	{
		int width = 800;
		int height = 600;
		std::string title = "Untitled Window";
		bool resizable = true;
		WindowMode mode = WindowMode::WINDOWED;
	};

	struct WindowState
	{
		int xpos;
		int ypos;
		int width;
		int height;
	};

	export class Window
	{
		GLFWwindow* m_handle = nullptr;
		int m_width{};
		int m_height{};
		std::string m_title{};
		WindowMode m_mode{};
		WindowState m_preFullscreenState{};

	public:
		explicit Window() = default;
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		Window(Window&& other) noexcept;
		Window& operator=(Window&& other) noexcept;

		/**
		 * Returns the native GLFWwindow instance for more control.
		 *
		 * @return Native window instance.
		 */
		[[nodiscard]] GLFWwindow* handle() const
		{
			return m_handle;
		}

		/**
		 * Displays the window.
		 */
		void show() const;

		void resize(int width, int height) const;

		[[nodiscard]] bool shouldClose() const;
		void setShouldClose(bool shouldClose) const;

		void setTitle(const std::string& title);
		[[nodiscard]] std::string getTitle() const;

		static void setResizable(bool resizable);
		void setWindowMode(WindowMode mode);

		[[nodiscard]] WindowMode getWindowMode() const;

		static void pollEvents();

		void attachMetalToGLFW() const;

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

			void setWindowMode(const WindowMode& mode)
			{
				// Todo
			}

			void attachMetalToGLFW()
			{
				m_rawWindow.attachMetalToGLFW();
			}
		};
	} // namespace raii
}     // namespace lune
