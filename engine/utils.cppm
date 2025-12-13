module;
#include <string>
export module lune:utils;

namespace lune
{
	/**
	 * @brief Returns the absolute path to the running executable.
	 *
	 * @note Works on macOS and Linux. Returns an empty path on failure.
	 *
	 * @return Absolute path to the executable.
	 */
	export std::string getWorkingDirectory();

	/**
	 * @brief Sets the working directory to be the root path of the executable.
	 * This is useful for when you want to use relative pathing.
	 */
	export void setWorkingDirectory();
} // namespace lune
