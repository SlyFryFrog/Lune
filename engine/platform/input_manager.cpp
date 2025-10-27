module;
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <initializer_list>
#include <map>
#include <memory>
#include <print>
#include <queue>
module lune;

namespace lune
{
	std::map<Key, std::shared_ptr<InputEvent>> InputManager::getEvents()
	{
		return m_events;
	}

	void InputManager::_processInputCallback(GLFWwindow* window, const int key, const int scancode,
											 const int action, const int mods)
	{
		// Ignore repeated events as they are updated in _process() when called externally
		if (action == GLFW_REPEAT)
		{
			return;
		}

		const Key libKey = static_cast<Key>(key);
		InputState state;

		if (action == GLFW_PRESS && !m_events.contains(libKey))
		{
			state = JUST_PRESSED;
		}
		else if (action == GLFW_RELEASE)
		{
			state = JUST_RELEASED;
		}

		const auto event = std::make_shared<InputEvent>(libKey, state);

		// Add to recent queue if just pressed
		// Used for handling combinations of keys being pressed
		m_recentQueue.push(event);

		m_eventQueue.push(event);
		m_events[libKey] = event;
	}

	void InputManager::_processMouseCallback(GLFWwindow* window, const double xposIn,
											 const double yposIn)
	{
		if (m_firstMouse)
		{
			m_prevMouseX = xposIn;
			m_prevMouseY = yposIn;
			m_firstMouse = false;
		}

		const double deltaX = xposIn - m_prevMouseX;
		const double deltaY = m_prevMouseY - yposIn;

		m_prevMouseX = xposIn;
		m_prevMouseY = yposIn;

		m_deltaX = deltaX;
		m_deltaY = deltaY;
	}

	void InputManager::_process()
	{
		// Handle all events in the queue by iterating through the queue and popping all events.
		// After popping an event from the eventQueue, the state of the key in the map is
		// either:
		// 1. Updated to reflect it being "PRESSED"
		// 2. Erased to reflect it being "RELEASED"
		while (!m_eventQueue.empty())
		{
			// Extract event from the eventQueue
			const std::shared_ptr<InputEvent> event = m_eventQueue.front();
			m_eventQueue.pop();

			Key key = event->getKey();
			const InputState state = event->getState();

			// Update the map depending on the event state
			if (state == JUST_PRESSED)
			{
				m_events[key]->setState(PRESSED);
			}
			else if (state == JUST_RELEASED)
			{
				m_events.erase(key); // Key is released, remove from active map
			}
		}

		clearRecentQueue();
	}

	bool InputManager::isPressed(const Key key)
	{
		if (!m_events.contains(key))
		{
			return false;
		}

		return m_events[key]->isPressed();
	}

	bool InputManager::isJustPressed(const Key key)
	{
		if (!m_events.contains(key))
		{
			return false;
		}

		return m_events[key]->isJustPressed();
	}

	bool InputManager::isJustReleased(const Key key)
	{
		if (!m_events.contains(key))
		{
			return false;
		}

		return m_events[key]->isJustReleased();
	}

	bool InputManager::isReleased(const Key key)
	{
		return !m_events.contains(key);
	}

	bool InputManager::isOrderedPressed(const std::initializer_list<Key>& keys)
	{
		return isOrderedPressed(std::vector<Key>(keys));
	}

	bool InputManager::isOrderedPressed(const std::vector<Key>& keys)
	{
		std::queue<std::shared_ptr<InputEvent>> tempRecentQueue = m_recentQueue;
		auto it = keys.begin();

		while (it != keys.end() && !tempRecentQueue.empty())
		{
			std::shared_ptr<InputEvent> currentEvent = tempRecentQueue.front();
			if (currentEvent->getKey() == *it &&
				(currentEvent->isPressed() || currentEvent->isJustPressed()))
			{
				tempRecentQueue.pop();
				++it;
			}
			else if (!currentEvent->isJustPressed())
			{
				tempRecentQueue.pop(); // Skip non-JUST_PRESSED events
			}
			else
			{
				return false; // Sequence doesn't match
			}
		}

		// If we've gone through all the keys in the desired sequence,
		// now check if all those keys are currently being held down.
		if (it == keys.end())
		{
			for (const Key& key : keys)
			{
				if (!m_events.contains(key) ||
					!(m_events[key]->isPressed() || m_events[key]->isJustPressed()))
				{
					return false; // Not all keys are held
				}
			}

			// Iterate through each Key, InputEvent
			for (const auto& [key, event] : m_events)
			{
				bool inCombo = false;
				for (const Key& comboKey : keys)
				{
					if (key == comboKey)
					{
						inCombo = true;
						break;
					}
				}

				if (!inCombo && event->isPressed())
				{
					return false; // Another key is pressed
				}
			}

			return true; // Sequence matched and all keys are held
		}

		return false; // The recent queue didn't contain the full sequence of presses
	}

	bool InputManager::isOrderedJustPressed(const std::initializer_list<Key>& keys)
	{
		return isOrderedJustPressed(std::vector(keys));
	}

	bool InputManager::isOrderedJustPressed(const std::vector<Key>& keys)
	{
		// Last key must just have been pressed for this to be valid
		if (!isJustPressed(keys.back()))
		{
			return false;
		}

		return isOrderedPressed(keys);
	}

	glm::vec2 InputManager::getMousePosition()
	{
		return {m_prevMouseX, m_prevMouseY};
	}

	glm::vec2 InputManager::getMouseDelta()
	{
		return {m_deltaX, m_deltaY};
	}

	void InputManager::clearRecentQueue()
	{
		// Clear recentQueue of all !is_pressed elements
		std::queue<std::shared_ptr<InputEvent>> tempQueue;
		while (!m_recentQueue.empty())
		{
			std::shared_ptr<InputEvent> event = m_recentQueue.front();
			m_recentQueue.pop();

			// Copy element to other queue
			if (event->isPressed()) // Checks if event contains key and is pressed
			{
				tempQueue.emplace(event);
			}
		}

		// Update queue
		m_recentQueue = tempQueue;
	}
} // namespace lune
