#include "SearchPathSearchOptions.h"

SearchPathSearchOptions::SearchPathSearchOptions()
{
    m_should_include_subdirectories = true;
    m_disk_files_only = false;
    m_absolute_paths = false;
    m_filter_extensions = false;
}

SearchPathSearchOptions& SearchPathSearchOptions::IncludeSubdirectories(const bool value)
{
    m_should_include_subdirectories = value;

    return *this;
}

SearchPathSearchOptions& SearchPathSearchOptions::OnlyDiskFiles(const bool value)
{
    m_disk_files_only = value;

    return *this;
}

SearchPathSearchOptions& SearchPathSearchOptions::AbsolutePaths(const bool value)
{
    m_absolute_paths = value;

    return *this;
}

SearchPathSearchOptions& SearchPathSearchOptions::FilterExtensions(std::string extension)
{
    m_extension = std::move(extension);
    m_filter_extensions = true;

    if(m_extension[0] != '.')
        m_extension = "." + m_extension;

    return *this;
}
