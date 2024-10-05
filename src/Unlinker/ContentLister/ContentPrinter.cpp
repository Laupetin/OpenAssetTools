#include "ContentPrinter.h"

#include <format>
#include <iostream>

ContentPrinter::ContentPrinter(Zone* zone)
{
    m_zone = zone;
}

void ContentPrinter::PrintContent() const
{
    const auto* pools = m_zone->m_pools.get();
    std::cout << std::format("Zone '{}' ({})\n", m_zone->m_name, m_zone->m_game->GetShortName());
    std::cout << "Content:\n";

    for (const auto& asset : *pools)
        std::cout << std::format("{}, {}\n", *pools->GetAssetTypeName(asset->m_type), asset->m_name);

    std::cout << "\n";
}
