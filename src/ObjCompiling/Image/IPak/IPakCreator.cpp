#include "IPakCreator.h"

#include <format>
#include <iostream>

void IPakCreator::AddImage(std::string imageName)
{
    m_image_names.emplace_back(std::move(imageName));
}

void IPakCreator::Finalize(ISearchPath& searchPath, const std::filesystem::path& outPath)
{
    std::cout << std::format("Creating ipak with {} entries:\n", m_image_names.size());

    for (const auto& imageName : m_image_names)
        std::cout << std::format("  {}\n", imageName);
}
