#include "IPakCreator.h"

#include <format>
#include <iostream>

IPakToCreate::IPakToCreate(std::string name)
    : m_name(std::move(name))
{
}

void IPakToCreate::AddImage(std::string imageName)
{
    m_image_names.emplace_back(std::move(imageName));
}

void IPakToCreate::Build(ISearchPath& searchPath, const std::filesystem::path& outPath)
{
    std::cout << std::format("Creating ipak {} with {} entries:\n", m_name, m_image_names.size());

    for (const auto& imageName : m_image_names)
        std::cout << std::format("  {}\n", imageName);
}

IPakToCreate* IPakCreator::GetOrAddIPak(const std::string& ipakName)
{
    const auto existingIPak = m_ipak_lookup.find(ipakName);
    if (existingIPak != m_ipak_lookup.end())
        return existingIPak->second;

    auto newIPak = std::make_unique<IPakToCreate>(ipakName);
    auto* result = newIPak.get();
    m_ipaks.emplace_back(std::move(newIPak));

    return result;
}

void IPakCreator::Finalize(ISearchPath& searchPath, const std::filesystem::path& outPath)
{
    for (const auto& ipakToCreate : m_ipaks)
        ipakToCreate->Build(searchPath, outPath);
}
