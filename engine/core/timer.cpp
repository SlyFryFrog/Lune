module;
#include <chrono>
module lune;

namespace lune
{
	void Timer::start()
	{
		m_start = std::chrono::high_resolution_clock::now();
		m_prevTime = m_start;
	}

	double Timer::delta()
	{
		const auto now{std::chrono::high_resolution_clock::now()};
		const double time{
				std::chrono::duration_cast<std::chrono::duration<double>>(now - m_prevTime)
						.count()};
		m_prevTime = now;

		return time;
	}

	double Timer::peakDelta() const
	{
		const auto now{std::chrono::high_resolution_clock::now()};
		return std::chrono::duration_cast<std::chrono::duration<double>>(now - m_prevTime).count();
	}

	double Timer::elapsed()
	{
		const auto now{std::chrono::high_resolution_clock::now()};
		const double time{
				std::chrono::duration_cast<std::chrono::duration<double>>(now - m_start).count()};
		m_start = now;
		return time;
	}

	double Timer::peakElapsed() const
	{
		const auto now{std::chrono::high_resolution_clock::now()};
		return std::chrono::duration_cast<std::chrono::duration<double>>(now - m_start).count();
	}
} // namespace lune
