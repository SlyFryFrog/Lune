module;
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <initializer_list>
#include <map>
#include <memory>
#include <queue>
export module lune:input_manager;

export import :input_event;
export import :input_map;

namespace lune
{
	/**
	 * @brief Manages input events, key states, and mouse interactions.
	 *
	 * The `InputManager` class provides static methods to query the state of keys,
	 * check for key combinations, and retrieve mouse position and movement.
	 * It is designed to be used globally and does not require instantiation.
	 */
	export class InputManager
	{
		static inline std::queue<std::shared_ptr<InputEvent>> m_eventQueue;
		static inline std::queue<std::shared_ptr<InputEvent>> m_recentQueue;
		static inline std::map<Key, std::shared_ptr<InputEvent>> m_events;

		static inline double m_deltaX;
		static inline double m_deltaY;
		static inline double m_prevMouseX;
		static inline double m_prevMouseY;

		static inline bool m_firstMouse = true;

	public:
		/**
		 * @brief Gets a map of all active input events.
		 *
		 * @return std::map<Key, std::shared_ptr<InputEvent>> A map of key to input event pairs.
		 */
		static std::map<Key, std::shared_ptr<InputEvent>> getEvents();

		/**
		 * @brief Internally called by GLFW when a key event occurs.
		 *
		 * @param window The GLFW window instance.
		 * @param key The key that was pressed or released.
		 * @param scancode The system-specific scancode of the key.
		 * @param action The action (press, release, repeat).
		 * @param mods Bit field describing which modifier keys were held down.
		 */
		static void _processInputCallback(GLFWwindow* window, int key, int scancode, int action,
										  int mods);

		/**
		 * @brief Internally called by GLFW when the mouse moves.
		 *
		 * @param window The GLFW window instance.
		 * @param xposIn The new X position of the mouse.
		 * @param yposIn The new Y position of the mouse.
		 */
		static void _processMouseCallback(GLFWwindow* window, double xposIn, double yposIn);

		/**
		 * @brief Processes and updates input states and events.
		 *
		 * This method is called internally by the Window class and should NOT be called directly.
		 * It updates the state of all input events and clears outdated events.
		 */
		static void _process();

		/**
		 * @brief Checks if a key is currently pressed.
		 *
		 * @param key The key to check.
		 * @return true If the key is pressed.
		 * @return false If the key is not pressed.
		 */
		static bool isPressed(Key key);

		/**
		 * @brief Checks if a key was pressed in the current frame.
		 *
		 * @param key The key to check.
		 * @return true If the key was pressed in the current frame.
		 * @return false If the key was not pressed in the current frame.
		 */
		static bool isJustPressed(Key key);

		/**
		 * @brief Checks if a key was released in the current frame.
		 *
		 * @param key The key to check.
		 * @return true If the key was released in the current frame.
		 * @return false If the key was not released in the current frame.
		 */
		static bool isJustReleased(Key key);

		/**
		 * @brief Checks if a key is currently released.
		 *
		 * @param key The key to check.
		 * @return true If the key is released.
		 * @return false If the key is pressed.
		 */
		static bool isReleased(Key key);

		/**
		 * @brief Checks if keys are pressed in a specific order.
		 *
		 * @param keys The ordered list of keys to check.
		 * @return true If all keys are pressed in the specified order.
		 * @return false If not all keys are pressed or if an extra key is pressed.
		 */
		static bool isOrderedPressed(const std::initializer_list<Key>& keys);

		/**
		 * @brief Checks if keys are pressed in a specific order.
		 *
		 * @param keys The ordered list of keys to check.
		 * @return true If all keys are pressed in the specified order.
		 * @return false If not all keys are pressed or if an extra key is pressed.
		 */
		static bool isOrderedPressed(const std::vector<Key>& keys);

		/**
		 * @brief Checks if keys were pressed in a specific order in the current frame.
		 *
		 * @param keys The ordered list of keys to check.
		 * @return true If all keys were pressed in the specified order in the current frame.
		 * @return false If not all keys were pressed in the current frame or if an extra key was
		 * pressed.
		 */
		static bool isOrderedJustPressed(const std::initializer_list<Key>& keys);

		/**
		 * @brief Checks if keys were pressed in a specific order in the current frame.
		 *
		 * @param keys The ordered list of keys to check.
		 * @return true If all keys were pressed in the specified order in the current frame.
		 * @return false If not all keys were pressed in the current frame or if an extra key was
		 * pressed.
		 */
		static bool isOrderedJustPressed(const std::vector<Key>& keys);

		/**
		 * @brief Gets the current mouse position.
		 *
		 * @return glm::vec2 The current mouse position in screen coordinates.
		 */
		static glm::vec2 getMousePosition();

		/**
		 * @brief Gets the mouse movement delta since the last frame.
		 *
		 * @return glm::vec2 The mouse movement delta (x, y) since the last frame.
		 */
		static glm::vec2 getMouseDelta();

	private:
		/**
		 * @brief Clears all elements from the recent queue that are not currently pressed.
		 *
		 * This method is called internally to clean up outdated events.
		 */
		static void clearRecentQueue();
	};
} // namespace lune
