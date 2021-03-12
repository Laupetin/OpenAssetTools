#include "ZoneCreatorT6.h"

#include "Game/T6/T6.h"
#include "Game/T6/GameT6.h"
#include "Game/T6/GameAssetPoolT6.h"

using namespace T6;

namespace T6
{
    class SpecifiedAsset
    {
    public:
        XAssetType m_type;
        std::string m_name;
        bool m_reference;
    };

    class AssetLoaderImpl
    {
        ZoneCreationContext& m_context;

        void CreateZoneAssetPools(Zone* zone) const
        {
            zone->m_pools = std::make_unique<GameAssetPoolT6>(zone, zone->m_priority);

            for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
                zone->m_pools->InitPoolDynamic(assetType);
        }

    public:
        explicit AssetLoaderImpl(ZoneCreationContext& context)
            : m_context(context)
        {
        }

        std::unique_ptr<Zone> CreateZoneForDefinition()
        {
            auto zone = std::make_unique<Zone>(m_context.m_zone_name, 0, &g_GameT6);
            CreateZoneAssetPools(zone.get());

            std::vector<SpecifiedAsset> specifiedAssets;

            return zone;
        }
    };
}

bool ZoneCreator::SupportsGame(const std::string& gameName) const
{
    return gameName == g_GameT6.GetShortName();
}

std::unique_ptr<Zone> ZoneCreator::CreateZoneForDefinition(ZoneCreationContext& context) const
{
    AssetLoaderImpl impl(context);
    return impl.CreateZoneForDefinition();
}
