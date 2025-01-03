#pragma once

#include "SearchPath/ISearchPath.h"

#include <unordered_map>

class MockSearchPath final : public ISearchPath
{
public:
    void AddFileData(std::string fileName, std::string fileData);

    SearchPathOpenFile Open(const std::string& fileName) override;
    const std::string& GetPath() override;
    void Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback) override;

private:
    std::unordered_map<std::string, std::string> m_file_data_map;
};
