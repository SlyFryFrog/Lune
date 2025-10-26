module;
export module lune:input_event;

import :input_map;

namespace lune
{
	/**
	 * @brief Stores the state of a give Key.
	 */
	export class InputEvent
	{
		Key m_key{};
		InputState m_state{RELEASED};

	public:
		InputEvent() = default;

		explicit InputEvent(const Key key) : m_key(key)
		{
		}

		explicit InputEvent(const Key key, const InputState state) : m_key(key), m_state(state)
		{
		}

		bool operator==(const Key key) const
		{
			return m_key == key;
		}

		[[nodiscard]] Key getKey() const
		{
			return m_key;
		}

		[[nodiscard]] bool isPressed() const
		{
			return m_state == PRESSED;
		}

		[[nodiscard]] bool isJustPressed() const
		{
			return m_state == JUST_PRESSED;
		}

		[[nodiscard]] bool isJustReleased() const
		{
			return m_state == JUST_RELEASED;
		}

		void setState(const InputState state)
		{
			m_state = state;
		}

		[[nodiscard]] InputState getState() const
		{
			return m_state;
		}
	};
} // namespace lune
