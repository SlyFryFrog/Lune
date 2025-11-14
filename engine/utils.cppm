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

	export void setWorkingDirectory();

	/**
	 * @brief Current working directory where the executable is located. Is determined at runtime.
	 */
	export const inline std::string WorkingDirectory = getWorkingDirectory();
}