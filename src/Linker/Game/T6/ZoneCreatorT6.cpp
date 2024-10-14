#include "ZoneCreatorT6.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/T6.h"
#include "IObjLoader.h"
#include "ObjLoading.h"

#include <format>
#include <iostream>

using namespace T6;

GameId ZoneCreator::GetGameId() const
{
    return GameId::T6;
}

asset_type_t ZoneCreator::GetImageAssetType() const
{
    return ASSET_TYPE_IMAGE;
}

void ZoneCreator::HandleMetadata(Zone& zone, const ZoneCreationContext& context) const
{
    std::vector<KeyValuePair> kvpList;

    for (const auto& metaData : context.m_definition->m_properties.m_properties)
    {
        if (metaData.first.rfind("level.", 0) == 0)
        {
            const std::string strValue = metaData.first.substr(std::char_traits<char>::length("level."));
            if (strValue.empty())
                continue;

            int keyHash;
            if (strValue[0] == '@')
            {
                char* endPtr;
                keyHash = strtol(&strValue[1], &endPtr, 16);

                if (endPtr != &strValue[strValue.size()])
                {
                    std::cerr << std::format("Could not parse metadata key \"{}\" as hash\n", metaData.first);
                    continue;
                }
            }
            else
            {
                keyHash = Common::Com_HashKey(strValue.c_str(), 64);
            }

            KeyValuePair kvp{keyHash, Common::Com_HashKey(zone.m_name.c_str(), 64), zone.GetMemory()->Dup(metaData.second.c_str())};
            kvpList.push_back(kvp);
        }
    }

    if (!kvpList.empty())
    {
        auto* kvps = zone.GetMemory()->Create<KeyValuePairs>();
        kvps->name = zone.GetMemory()->Dup(zone.m_name.c_str());
        kvps->numVariables = static_cast<int>(kvpList.size());
        kvps->keyValuePairs = zone.GetMemory()->Alloc<KeyValuePair>(kvpList.size());

        for (auto i = 0u; i < kvpList.size(); i++)
            kvps->keyValuePairs[i] = kvpList[i];

        zone.m_pools->AddAsset(std::make_unique<XAssetInfo<KeyValuePairs>>(ASSET_TYPE_KEYVALUEPAIRS, zone.m_name, kvps));
    }
}
