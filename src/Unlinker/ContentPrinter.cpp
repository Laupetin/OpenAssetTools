#include "ContentPrinter.h"

ContentPrinter::ContentPrinter(Zone* zone)
{
    m_zone = zone;
}

void ContentPrinter::PrintContent() const
{
    const ZoneContent content = m_zone->GetPools()->GetContent();

    printf("Zone '%s' (%s)\n", m_zone->m_name.c_str(), content.m_game_name.c_str());
    puts("Content:");

    for(const auto& asset : content.m_assets)
    {
        printf("%s, %s\n", asset.m_asset_type_name.c_str(), asset.m_asset_name.c_str());
    }

    puts("");
}
