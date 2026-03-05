#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>
#include <string>

namespace utils
{
    static constexpr uint32_t MakeMagic32(const char ch0, const char ch1, const char ch2, const char ch3)
    {
        return static_cast<uint32_t>(ch0) | static_cast<uint32_t>(ch1) << 8 | static_cast<uint32_t>(ch2) << 16 | static_cast<uint32_t>(ch3) << 24;
    }

    /**
     * \brief Splits a path string as user input into a list of paths.
     * \param pathsString The path string that was taken as user input.
     * \param output A set for strings to save the output to.
     * \return \c true if the user input was valid and could be processed successfully, otherwise \c false.
     */
    bool ParsePathsString(const std::string& pathsString, std::set<std::string>& output);

    enum class TextFileCheckDirtyResult : std::uint8_t
    {
        OUTPUT_WRITTEN,
        OUTPUT_WAS_UP_TO_DATE,
        FAILURE
    };

    class TextFileCheckDirtyOutput final
    {
    public:
        explicit TextFileCheckDirtyOutput(std::filesystem::path path);
        ~TextFileCheckDirtyOutput();
        TextFileCheckDirtyOutput(const TextFileCheckDirtyOutput& other) = delete;
        TextFileCheckDirtyOutput(TextFileCheckDirtyOutput&& other) noexcept = default;
        TextFileCheckDirtyOutput& operator=(const TextFileCheckDirtyOutput& other) = delete;
        TextFileCheckDirtyOutput& operator=(TextFileCheckDirtyOutput&& other) noexcept = default;

        bool Open();
        std::ostream& Stream();
        TextFileCheckDirtyResult Close();

    private:
        [[nodiscard]] bool FileIsDirty(const std::string& renderedContent) const;

        std::filesystem::path m_path;

        bool m_open;
        bool m_has_existing_file;
        std::ofstream m_file_stream;
        std::ostringstream m_memory;
    };
} // namespace utils
