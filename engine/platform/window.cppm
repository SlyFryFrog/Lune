module;
#define GLFW_INCLUDE_NONE // We're not using OpenGL, therefore we disable it
#include <GLFW/glfw3.h>
#include <string>
export module lune:window;

import :input_manager;
import lune.gfx;

namespace lune
{
	/**
	 * @brief Enumerates the possible window modes.
	 */
	export enum WindowMode
	{
		FULLSCREEN,			///< Fullscreen mode.
		WINDOWED,			///< Windowed mode.
		WINDOWED_FULLSCREEN ///< Windowed fullscreen (borderless) mode.
	};


	/**
	 * @brief Contains configuration options for window creation.
	 */
	export struct WindowCreateInfo
	{
		int width = 800;						///< The width of the window in pixels.
		int height = 600;						///< The height of the window in pixels.
		std::string title = "Untitled Window";	///< The title of the window.
		bool resizable = true;					///< Whether the window is resizable.
		WindowMode mode = WindowMode::WINDOWED; ///< The initial window mode.
	};


	/**
	 * @brief Manages a GLFW window and its associated rendering context.
	 *
	 * The `Window` class provides methods for creating, destroying, and interacting with a GLFW
	 * window. It supports Metal (on macOS) and Vulkan rendering contexts.
	 */
	export class Window
	{
		GLFWwindow* m_handle{};
		int m_width{};
		int m_height{};
		int m_frameBufferWidth{};
		int m_frameBufferHeight{};
		std::string m_title{};
		WindowMode m_mode{};

#ifdef USE_METAL
		std::shared_ptr<gfx::RenderSurface> m_surface{};
#endif

	public:
		explicit Window() = default;
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		Window(Window&& other) noexcept;
		Window& operator=(Window&& other) noexcept;

		/**
		 * @brief Returns the native GLFWwindow instance for more control.
		 *
		 * @return GLFWwindow* The native window handle.
		 */
		[[nodiscard]] GLFWwindow* handle() const
		{
			return m_handle;
		}

		/**
		 * @brief Displays the window.
		 */
		void show() const;

		/**
		 * @brief Resizes the window.
		 *
		 * @param width The new width in pixels.
		 * @param height The new height in pixels.
		 */
		void resize(int width, int height) const;

		/**
		 * @brief Checks if the window should close.
		 *
		 * @return true If the window should close.
		 * @return false If the window should remain open.
		 */
		[[nodiscard]] bool shouldClose() const;

		/**
		 * @brief Sets whether the window should close.
		 *
		 * @param shouldClose If true, the window will close.
		 */
		void setShouldClose(bool shouldClose) const;

		/**
		 * @brief Sets the window title.
		 *
		 * @param title The new title.
		 */
		void setTitle(const std::string& title);

		/**
		 * @brief Gets the current window title.
		 *
		 * @return std::string The window title.
		 */
		[[nodiscard]] std::string getTitle() const noexcept;

		/**
		 * @brief Sets whether the window is resizable.
		 *
		 * @param resizable If true, the window will be resizable.
		 */
		static void setResizable(bool resizable);

		/**
		 * @brief Sets the window mode.
		 *
		 * @param mode The new window mode.
		 */
		void setWindowMode(WindowMode mode);

		/**
		 * @brief Gets the current window mode.
		 *
		 * @return WindowMode The current window mode.
		 */
		[[nodiscard]] WindowMode getWindowMode() const noexcept;

		/**
		 * @brief Polls for window events.
		 *
		 * This method should be called regularly to process events.
		 */
		static void pollEvents();

		/**
		 * @brief Creates the window using the provided configuration.
		 *
		 * @param info The window creation info.
		 */
		void create(const WindowCreateInfo& info);

		/**
		 * @brief Destroys the window and releases associated resources.
		 */
		void destroy();

		[[nodiscard]] int width() const
		{
			return m_width;
		}

		[[nodiscard]] int height() const
		{
			return m_height;
		}

		[[nodiscard]] gfx::RenderSurface& surface() const
		{
			return *m_surface;
		}

	private:
		/**
		 * @brief Callback for framebuffer resize events.
		 *
		 * @param handle The GLFW window handle.
		 * @param width The new width in pixels.
		 * @param height The new height in pixels.
		 */
		static void _onFrameBufferSizeCallback(GLFWwindow* handle, int width, int height);

#ifdef USE_METAL
		/**
		 * @brief Attaches a Metal layer to the GLFW window (macOS only).
		 */
		void attachMetalToGLFW();

		#elif USE_VULKAN
		/**
		 * @brief Attaches a Vulkan context to the GLFW window.
		 */
		void attachVulkanToGLFW();
#endif
	};


	namespace raii
	{
		/**
		 * @brief RAII wrapper for the `Window` class.
		 *
		 * Automatically destroys the window when the wrapper goes out of scope.
		 */
		export class Window
		{
			lune::Window m_rawWindow;

		public:
			/**
			 * @brief Constructs a new RAII Window.
			 *
			 * @param createInfo The window creation info.
			 */
			explicit Window(const WindowCreateInfo& createInfo)
			{
				m_rawWindow.create(createInfo);
			}

			/**
			 * @brief Returns the native GLFWwindow instance.
			 *
			 * @return GLFWwindow* The native window handle.
			 */
			[[nodiscard]] GLFWwindow* handle() const
			{
				return m_rawWindow.handle();
			}

			/**
			 * @brief Displays the window.
			 */
			void show() const
			{
				m_rawWindow.show();
			}

			/**
			 * @brief Resizes the window.
			 *
			 * @param width The new width in pixels.
			 * @param height The new height in pixels.
			 */
			void resize(const int width, const int height) const
			{
				m_rawWindow.resize(width, height);
			}

			/**
			 * @brief Checks if the window should close.
			 *
			 * @return true If the window should close.
			 * @return false If the window should remain open.
			 */
			[[nodiscard]] bool shouldClose() const
			{
				return m_rawWindow.shouldClose();
			}

			/**
			 * @brief Sets whether the window should close.
			 *
			 * @param shouldClose If true, the window will close.
			 */
			void setShouldClose(const bool shouldClose) const
			{
				m_rawWindow.setShouldClose(shouldClose);
			}

			/**
			 * @brief Sets the window mode.
			 *
			 * @param mode The new window mode.
			 */
			void setWindowMode(const WindowMode& mode)
			{
				m_rawWindow.setWindowMode(mode);
			}

			[[nodiscard]] int width() const
			{
				return m_rawWindow.width();
			}

			[[nodiscard]] int height() const
			{
				return m_rawWindow.height();
			}

			[[nodiscard]] gfx::RenderSurface& surface() const
			{
				return m_rawWindow.surface();
			}
		};
	} // namespace raii
} // namespace lune
