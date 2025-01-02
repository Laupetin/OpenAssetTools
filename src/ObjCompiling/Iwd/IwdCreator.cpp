#include "IwdCreator.h"

#include "Utils/FileToZlibWrapper.h"

#include <format>
#include <iostream>

IwdToCreate::IwdToCreate(std::string name)
    : m_name(std::move(name))
{
}

void IwdToCreate::AddFile(std::string filePath)
{
    m_file_paths.emplace_back(std::move(filePath));
}

void IwdToCreate::Build(ISearchPath& searchPath, const std::filesystem::path& outPath)
{
    std::cout << std::format("Creating iwd {} with {} entries:\n", m_name, m_file_paths.size());

    for (const auto& filePath : m_file_paths)
        std::cout << std::format("  {}\n", filePath);
}

IwdToCreate* IwdCreator::GetOrAddIwd(const std::string& iwdName)
{
    const auto existingIwd = m_iwd_lookup.find(iwdName);
    if (existingIwd != m_iwd_lookup.end())
        return existingIwd->second;

    auto newIwd = std::make_unique<IwdToCreate>(iwdName);
    auto* result = newIwd.get();
    m_iwds.emplace_back(std::move(newIwd));

    return result;
}

void IwdCreator::Finalize(ISearchPath& searchPath, const std::filesystem::path& outPath)
{
    for (const auto& iwdToCreate : m_iwds)
        iwdToCreate->Build(searchPath, outPath);
}
