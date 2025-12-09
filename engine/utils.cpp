module;
#include <filesystem>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif
module lune;

namespace lune
{
	std::string getWorkingDirectory()
	{
		// macOS doesn't use the pwd of the executable when launching from
		// clicking on the executable directly.
#if defined(__APPLE__)
		char buffer[PATH_MAX];
		uint32_t size{sizeof(buffer)};
		if (_NSGetExecutablePath(buffer, &size) == 0)
		{
			return std::filesystem::canonical(buffer).parent_path().string();
		}
#endif
		// Default implementation
		return std::filesystem::current_path().string();
	}

	void setWorkingDirectory()
	{
		std::filesystem::current_path(getWorkingDirectory());
	}
} // namespace lune
