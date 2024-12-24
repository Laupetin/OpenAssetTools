#include "MockSearchPath.h"

#include <sstream>

void MockSearchPath::AddFileData(std::string fileName, std::string fileData)
{
    m_file_data_map.emplace(std::make_pair(std::move(fileName), std::move(fileData)));
}

SearchPathOpenFile MockSearchPath::Open(const std::string& fileName)
{
    const auto foundFileData = m_file_data_map.find(fileName);

    if (foundFileData == m_file_data_map.end())
        return {};

    return {std::make_unique<std::istringstream>(foundFileData->second), foundFileData->second.size()};
}

const std::string& MockSearchPath::GetPath()
{
    const static std::string NAME = "MockFiles";
    return NAME;
}

void MockSearchPath::Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback) {}
