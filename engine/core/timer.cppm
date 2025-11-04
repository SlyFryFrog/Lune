module;
#include <chrono>
export module lune:timer;

namespace lune
{
	/**
	 * @brief A high-resolution timer for measuring time intervals.
	 */
	export class Timer
	{
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_prevTime;

	public:
		Timer() = default;
		~Timer() = default;

		/**
		 * @brief Resets the timer to the current time.
		 *
		 * Sets both `m_start` and `m_prevTime` to the current time,
		 * effectively starting or restarting the timer.
		 */
		void start();

		/**
		 * @brief Gets the time elapsed since the last call to `delta()`.
		 *
		 * Updates `m_prevTime` to the current time, preparing for the next `delta()` call.
		 * Use this method to measure frame time in game loops.
		 *
		 * @return The time (in seconds) since the last call to `delta()`.
		 */
		[[nodiscard]] double delta();

		/**
		 * @brief Gets the time since the last `delta()` call without updating the timer.
		 *
		 * Unlike `delta()`, this method does not modify `m_prevTime`.
		 * Useful for checking the current frame time without affecting future calls.
		 *
		 * @return The time (in seconds) since the last `delta()` call.
		 */
		[[nodiscard]] double peakDelta() const;

		/**
		 * @brief Gets the total time elapsed since the timer was started or reset.
		 *
		 * Updates `m_prevTime` to the current time, preparing for the next `delta()` call.
		 * Use this method to measure the total elapsed time.
		 *
		 * @return The total time (in seconds) since the timer was started.
		 */
		[[nodiscard]] double elapsed();

		/**
		 * @brief Gets the total time elapsed since the timer was started, without updating the timer.
		 *
		 * Unlike `elapsed()`, this method does not modify `m_prevTime`.
		 * Useful for checking the total elapsed time without affecting future calls.
		 *
		 * @return The total time (in seconds) since the timer was started.
		 */
		[[nodiscard]] double peakElapsed() const;
	};
} // namespace lune
