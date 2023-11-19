#pragma once
#include "SearchPath/ISearchPath.h"

#include <map>

class MockSearchPath final : public ISearchPath
{
    std::map<std::string, std::string> m_file_data_map;

public:
    void AddFileData(std::string fileName, std::string fileData);

    SearchPathOpenFile Open(const std::string& fileName) override;
    std::string GetPath() override;
    void Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback) override;
};
