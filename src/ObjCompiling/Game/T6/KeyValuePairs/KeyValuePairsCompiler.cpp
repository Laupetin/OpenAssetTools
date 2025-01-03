#include "KeyValuePairsCompiler.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/T6.h"

#include <format>
#include <iostream>

using namespace T6;

KeyValuePairsCompiler::KeyValuePairsCompiler(const Zone& zone, const ZoneDefinition& zoneDefinition)
    : m_zone(zone),
      m_zone_definition(zoneDefinition)
{
}

std::optional<asset_type_t> KeyValuePairsCompiler::GetHandlingAssetType() const
{
    return std::nullopt;
}

AssetCreationResult KeyValuePairsCompiler::CreateAsset(const std::string& assetName, AssetCreationContext& context)
{
    return AssetCreationResult::NoAction();
}

void KeyValuePairsCompiler::FinalizeZone(AssetCreationContext& context)
{
    std::vector<KeyValuePair> kvpList;
    auto& memory = *m_zone.GetMemory();

    for (const auto& metaData : m_zone_definition.m_properties.m_properties)
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

            KeyValuePair kvp{keyHash, Common::Com_HashKey(m_zone.m_name.c_str(), 64), memory.Dup(metaData.second.c_str())};
            kvpList.push_back(kvp);
        }
    }

    if (!kvpList.empty())
    {
        auto* kvps = memory.Create<KeyValuePairs>();
        kvps->name = memory.Dup(m_zone.m_name.c_str());
        kvps->numVariables = static_cast<int>(kvpList.size());
        kvps->keyValuePairs = m_zone.GetMemory()->Alloc<KeyValuePair>(kvpList.size());

        for (auto i = 0u; i < kvpList.size(); i++)
            kvps->keyValuePairs[i] = kvpList[i];

        context.AddAsset(AssetRegistration<AssetKeyValuePairs>(m_zone.m_name, kvps));
    }
}
