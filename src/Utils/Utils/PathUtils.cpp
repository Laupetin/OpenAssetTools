#include "PathUtils.h"

#include <filesystem>

namespace utils
{
    std::string Path::GetFilename(const std::string& pathInput)
    {
        const std::filesystem::path path(pathInput);

        return path.filename().string();
    }

    std::string Path::GetFilenameWithoutExtension(const std::string& pathInput)
    {
        const std::filesystem::path path(pathInput);

        return path.filename().replace_extension().string();
    }

    std::string Path::GetExtension(const std::string& pathInput)
    {
        const std::filesystem::path path(pathInput);

        return path.extension().string();
    }

    std::string Path::GetDirectory(const std::string& pathInput)
    {
        const std::filesystem::path path(pathInput);

        return path.relative_path().string();
    }

    std::string Path::Combine(const std::string& p1, const std::string& p2)
    {
        std::filesystem::path path(p1);

        return path.append(p2).string();
    }
}
