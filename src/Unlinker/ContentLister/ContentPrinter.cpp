#include "ContentPrinter.h"

#include <format>
#include <iostream>

ContentPrinter::ContentPrinter(const Zone& zone)
    : m_zone(zone)
{
}

void ContentPrinter::PrintContent() const
{
    const auto* pools = m_zone.m_pools.get();
    const auto* game = IGame::GetGameById(m_zone.m_game_id);
    std::cout << std::format("Zone '{}' ({})\n", m_zone.m_name, game->GetShortName());
    std::cout << "Content:\n";

    for (const auto& asset : *pools)
        std::cout << std::format("{}, {}\n", *pools->GetAssetTypeName(asset->m_type), asset->m_name);

    std::cout << "\n";
}
