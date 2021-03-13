#include "SearchPathFilesystem.h"

#include <filesystem>
#include <fstream>

#include "Utils/ObjFileStream.h"

namespace fs = std::filesystem;

SearchPathFilesystem::SearchPathFilesystem(std::string path)
{
    m_path = std::move(path);
}

std::string SearchPathFilesystem::GetPath()
{
    return m_path;
}

SearchPathOpenFile SearchPathFilesystem::Open(const std::string& fileName)
{
    const auto filePath = fs::path(m_path).append(fileName);
    std::ifstream file(filePath.string(), std::fstream::in | std::fstream::binary);

    if (file.is_open())
    {
        return SearchPathOpenFile(std::make_unique<std::ifstream>(std::move(file)), static_cast<int64_t>(file_size(filePath)));
    }

    return SearchPathOpenFile();
}

void SearchPathFilesystem::Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback)
{
    try
    {
        if (options.m_should_include_subdirectories)
        {
            std::filesystem::recursive_directory_iterator iterator(m_path);
            for (const auto entry = begin(iterator); iterator != end(iterator); ++iterator)
            {
                auto path = entry->path();
                if (options.m_filter_extensions && path.extension().string() != options.m_extension)
                    continue;
                callback(options.m_absolute_paths ? absolute(path).string() : path.string());
            }
        }
        else
        {
            std::filesystem::directory_iterator iterator(m_path);
            for (const auto entry = begin(iterator); iterator != end(iterator); ++iterator)
            {
                auto path = entry->path();
                if (options.m_filter_extensions && path.extension().string() != options.m_extension)
                    continue;
                callback(options.m_absolute_paths ? absolute(path).string() : path.string());
            }
        }
    }
    catch (std::filesystem::filesystem_error& e)
    {
        printf("Directory Iterator threw error when trying to find files: \"%s\"\n", e.what());
    }
}
