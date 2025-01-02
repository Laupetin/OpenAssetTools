#include "IwdCreator.h"

#include <format>
#include <iostream>

void IwdCreator::AddFile(std::string filePath)
{
    m_file_paths.emplace_back(std::move(filePath));
}

void IwdCreator::Finalize(ISearchPath& searchPath, const std::filesystem::path& outPath)
{
    std::cout << std::format("Creating iwd with {} entries:\n", m_file_paths.size());

    for (const auto& filePath : m_file_paths)
        std::cout << std::format("  {}\n", filePath);
}
