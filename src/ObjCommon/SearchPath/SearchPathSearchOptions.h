#pragma once

#include <string>

class SearchPathSearchOptions
{
public:
    bool m_should_include_subdirectories;
    bool m_disk_files_only;
    bool m_absolute_paths;

    bool m_filter_extensions;
    std::string m_extension;

    SearchPathSearchOptions();

    SearchPathSearchOptions& IncludeSubdirectories(bool value);
    SearchPathSearchOptions& OnlyDiskFiles(bool value);
    SearchPathSearchOptions& AbsolutePaths(bool value);
    SearchPathSearchOptions& FilterExtensions(std::string extension);
};
