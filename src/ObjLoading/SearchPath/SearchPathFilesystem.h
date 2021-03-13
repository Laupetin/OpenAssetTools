#pragma once

#include <string>

#include "ISearchPath.h"

class SearchPathFilesystem final : public ISearchPath
{
    std::string m_path;

public:
    explicit SearchPathFilesystem(std::string path);

    SearchPathOpenFile Open(const std::string& fileName) override;
    std::string GetPath() override;
    void Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback) override;
};