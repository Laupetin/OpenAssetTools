#include "SearchPathFilesystem.h"

#include "Utils/Logging/Log.h"
#include "Utils/ObjFileStream.h"

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>

namespace fs = std::filesystem;

namespace
{
    bool CheckOptionsPrefix(std::string& prefix, std::string& path)
    {
        if (prefix.empty())
            return true;

        const auto combinedPath = fs::path(path) / prefix;
        if (fs::status(combinedPath).type() == std::filesystem::file_type::directory)
        {
            prefix = std::string();
            path = combinedPath.string();
            return true;
        }

        const auto parentDir = combinedPath.parent_path();
        if (!parentDir.empty())
        {
            if (fs::status(parentDir).type() != std::filesystem::file_type::directory)
                return false;

            path = parentDir.string();
            prefix = combinedPath.filename().string();
        }

        return true;
    }
} // namespace

SearchPathFilesystem::SearchPathFilesystem(std::string path)
    : m_path(std::move(path))
{
}

const std::string& SearchPathFilesystem::GetPath()
{
    return m_path;
}

SearchPathOpenFile SearchPathFilesystem::Open(const std::string& fileName)
{
    const auto filePath = fs::path(m_path).append(fileName);
    std::ifstream file(filePath.string(), std::fstream::in | std::fstream::binary);

    if (file.is_open())
        return SearchPathOpenFile(std::make_unique<std::ifstream>(std::move(file)), static_cast<int64_t>(file_size(filePath)));

    return SearchPathOpenFile();
}

void SearchPathFilesystem::Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback)
{
    std::string prefix = options.m_prefix;
    std::string path = m_path;
    if (!CheckOptionsPrefix(prefix, path))
        return;

    try
    {
        if (options.m_should_include_subdirectories)
        {
            std::filesystem::recursive_directory_iterator iterator(path);
            for (const auto entry = begin(iterator); iterator != end(iterator); ++iterator)
            {
                if (!entry->is_regular_file())
                    continue;

                auto entryPath = entry->path();
                if (!prefix.empty() && !entryPath.filename().string().starts_with(prefix))
                    continue;

                if (options.m_filter_extensions && entryPath.extension().string() != options.m_extension)
                    continue;

                const auto absolutePath = absolute(entryPath);
                if (options.m_absolute_paths)
                {
                    callback(absolutePath.string());
                }
                else
                {
                    const auto relativePath = fs::relative(entryPath, m_path);
                    callback(relativePath.string());
                }
            }
        }
        else
        {
            std::filesystem::directory_iterator iterator(path);
            for (const auto entry = begin(iterator); iterator != end(iterator); ++iterator)
            {
                if (!entry->is_regular_file())
                    continue;

                auto entryPath = entry->path();
                if (!prefix.empty() && !entryPath.filename().string().starts_with(prefix))
                    continue;

                if (options.m_filter_extensions && entryPath.extension().string() != options.m_extension)
                    continue;

                const auto absolutePath = absolute(entryPath);
                if (options.m_absolute_paths)
                {
                    callback(absolutePath.string());
                }
                else
                {
                    const auto relativePath = fs::relative(absolutePath, m_path);
                    callback(relativePath.string());
                }
            }
        }
    }
    catch (std::filesystem::filesystem_error& e)
    {
        con::error("Directory Iterator threw error when trying to find files: \"{}\"", e.what());
    }
}
