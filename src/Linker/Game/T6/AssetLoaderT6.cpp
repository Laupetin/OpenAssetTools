#include "AssetLoaderT6.h"

#include "Game/T6/T6.h"
#include "Game/T6/GameT6.h"
#include "Game/T6/GameAssetPoolT6.h"

using namespace T6;

bool AssetLoader::SupportsGame(const std::string& gameName) const
{
    return gameName == g_GameT6.GetShortName();
}

std::unique_ptr<Zone> AssetLoader::CreateZoneForDefinition(AssetLoadingContext& context) const
{
    auto zone = std::make_unique<Zone>(context.m_zone_name, 0, &g_GameT6);
    zone->m_pools = std::make_unique<GameAssetPoolT6>(zone.get(), zone->m_priority);

    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        zone->m_pools->InitPoolDynamic(assetType);

    return zone;
}
