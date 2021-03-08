#pragma once
#include <cstdint>
#include <set>
#include <string>

class FileUtils
{
public:
    static constexpr uint32_t MakeMagic32(const char ch0, const char ch1, const char ch2, const char ch3)
    {
        return static_cast<uint32_t>(ch0)
            | static_cast<uint32_t>(ch1) << 8
            | static_cast<uint32_t>(ch2) << 16
            | static_cast<uint32_t>(ch3) << 24;
    }

    /**
     * \brief Splits a path string as user input into a list of paths.
     * \param pathsString The path string that was taken as user input.
     * \param output A set for strings to save the output to.
     * \return \c true if the user input was valid and could be processed successfully, otherwise \c false.
     */
    static bool ParsePathsString(const std::string& pathsString, std::set<std::string>& output);
};
