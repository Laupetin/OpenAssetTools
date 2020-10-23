#include "ContentPrinter.h"

ContentPrinter::ContentPrinter(Zone* zone)
{
    m_zone = zone;
}

void ContentPrinter::PrintContent() const
{
    const auto* pools = m_zone->m_pools.get();
    printf("Zone '%s' (%s)\n", m_zone->m_name.c_str(), m_zone->m_game->GetName().c_str());
    puts("Content:");

    for(const auto& asset : *pools)
    {
        printf("%s, %s\n", pools->GetAssetTypeName(asset->m_type).c_str(), asset->m_name.c_str());
    }

    puts("");
}
