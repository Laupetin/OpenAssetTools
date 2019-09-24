#include "PathUtils.h"

namespace utils
{
    std::string Path::GetFilename(std::string path)
    {
        const size_t lastSlashIndex = path.find_last_of("\\/");
        if (std::string::npos != lastSlashIndex)
        {
            path.erase(0, lastSlashIndex);
        }

        return path;
    }

    std::string Path::GetFilenameWithoutExtension(std::string path)
    {
        const size_t lastSlashIndex = path.find_last_of("\\/");
        if (std::string::npos != lastSlashIndex)
        {
            path.erase(0, lastSlashIndex + 1);
        }

        // Remove extension if present.
        const size_t dotIndex = path.rfind('.');
        if (std::string::npos != dotIndex)
        {
            path.erase(dotIndex);
        }

        return path;
    }

    std::string Path::GetExtension(std::string path)
    {
        const size_t lastSlashIndex = path.find_last_of("\\/");
        const size_t lastDotIndex = path.find_last_of('.');
        if (std::string::npos != lastDotIndex
            && (lastSlashIndex == std::string::npos || lastDotIndex > lastSlashIndex))
        {
            path.erase(0, lastDotIndex);

            return path;
        }

        return "";
    }

    std::string Path::GetDirectory(std::string path)
    {
        const size_t lastSlashIndex = path.find_last_of("\\/");
        if (std::string::npos != lastSlashIndex)
        {
            path.erase(lastSlashIndex);
        }
        else
        {
            return "./";
        }

        return path;
    }

    std::string Path::Combine(std::string p1, std::string p2)
    {
        char c;

        while (!p1.empty() && (c = p1[p1.size() - 1], c == '\\' || c == '/'))
            p1.erase(p1.size() - 1);

        while (!p2.empty() && (c = p2[0], c == '\\' || c == '/'))
            p2.erase(0);

        if (!p1.empty())
            p1 += '/';

        return p1 + p2;
    }
}
