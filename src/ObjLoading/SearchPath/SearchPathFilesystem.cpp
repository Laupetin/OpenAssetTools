#include "SearchPathFilesystem.h"
#include "Utils/PathUtils.h"

#include <filesystem>

SearchPathFilesystem::SearchPathFilesystem(std::string path)
{
    m_path = std::move(path);
}

const std::string& SearchPathFilesystem::GetPath() const
{
    return m_path;
}

FileAPI::IFile* SearchPathFilesystem::Open(const std::string& fileName)
{
    FileAPI::File file = FileAPI::Open(utils::Path::Combine(m_path, fileName), FileAPI::Mode::MODE_READ);

    if (file.IsOpen())
    {
        return new FileAPI::File(std::move(file));
    }

    return nullptr;
}


void SearchPathFilesystem::FindAll(const std::function<void(const std::string&)> callback)
{
    std::filesystem::recursive_directory_iterator iterator(m_path);

    for (const auto entry = begin(iterator); iterator != end(iterator); ++iterator)
    {
        callback(entry->path().string());
    }
}

void SearchPathFilesystem::FindAllOnDisk(const std::function<void(const std::string&)> callback)
{
    std::filesystem::recursive_directory_iterator iterator(m_path);

    for (const auto entry = begin(iterator); iterator != end(iterator); ++iterator)
    {
        callback(absolute(entry->path()).string());
    }
}

void SearchPathFilesystem::FindByExtension(const std::string& extension,
                                           const std::function<void(const std::string&)> callback)
{
    std::filesystem::recursive_directory_iterator iterator(m_path);

    for (const auto entry = begin(iterator); iterator != end(iterator); ++iterator)
    {
        auto entryPath = entry->path();

        if (entryPath.extension().string() == extension)
        {
            callback(entryPath.string());
        }
    }
}

void SearchPathFilesystem::FindOnDiskByExtension(const std::string& extension,
                                                 const std::function<void(const std::string&)> callback)
{
    std::filesystem::recursive_directory_iterator iterator(m_path);

    for (const auto entry = begin(iterator); iterator != end(iterator); ++iterator)
    {
        auto entryPath = entry->path();

        if (entryPath.extension().string() == extension)
        {
            callback(absolute(entryPath).string());
        }
    }
}
