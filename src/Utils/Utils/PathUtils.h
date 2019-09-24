#pragma once

#include <string>

namespace utils
{
    class Path
    {
    public:
        static std::string GetFilename(std::string path);
        static std::string GetFilenameWithoutExtension(std::string path);
        static std::string GetExtension(std::string path);
        static std::string GetDirectory(std::string path);
        static std::string Combine(std::string p1, std::string p2);
    };
}