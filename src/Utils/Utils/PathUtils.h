#pragma once

#include <string>

namespace utils
{
    class Path
    {
    public:
        static std::string GetFilename(const std::string& pathInput);
        static std::string GetFilenameWithoutExtension(const std::string& pathInput);
        static std::string GetExtension(const std::string& pathInput);
        static std::string GetDirectory(const std::string& pathInput);
        static std::string Combine(const std::string& p1, const std::string& p2);
    };
}