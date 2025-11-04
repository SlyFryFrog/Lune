module;
#include <optional>
#include <string>
export module lune:file;

namespace lune
{
	export class File
	{
	public:
		/**
		 * @brief Reads the contents of a file and returns std::optional<std::string> to check the
		 * success of the read.
		 *
		 * @param path	Path to the file.
		 *
		 * @return std::optional containing the file contents if the file was
		 * successfully opened and read; std::nullopt otherwise.
		 */
		static std::optional<std::string> read(const std::string& path);

		/**
		 * @brief Overwrites all data in the file with the new content.
		 *
		 * @param path Path to the file.
		 * @param content std::string being written to the file. Overwrites all data.
		 */
		static void write(const std::string& path, const std::string& content);

		/**
		 * @brief Appends content to the desired file.
		 *
		 * @param path Path to the file.
		 * @param content std::string being appended to the file.
		 */
		static void append(const std::string& path, const std::string& content);
	};
} // namespace lune
