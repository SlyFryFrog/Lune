module;
#include <filesystem>
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

	/**
	 * @brief Current working directory where the executable is located. Is determined at runtime.
	 */
	export const inline std::string WorkingDirectory = getWorkingDirectory();
}
