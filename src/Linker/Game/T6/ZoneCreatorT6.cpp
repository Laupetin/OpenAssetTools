#include "ZoneCreatorT6.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "Game/T6/GameT6.h"
#include "Game/T6/T6.h"
#include "ObjLoading.h"

#include <iostream>

using namespace T6;

ZoneCreator::ZoneCreator()
{
    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
    {
        AddAssetTypeName(assetType, GameAssetPoolT6::AssetTypeNameByType(assetType));
    }
}

void ZoneCreator::AddAssetTypeName(asset_type_t assetType, std::string name)
{
    m_asset_types_by_name.emplace(std::make_pair(std::move(name), assetType));
}

std::vector<Gdt*> ZoneCreator::CreateGdtList(const ZoneCreationContext& context)
{
    std::vector<Gdt*> gdtList;
    gdtList.reserve(context.m_gdt_files.size());
    for (const auto& gdt : context.m_gdt_files)
        gdtList.push_back(gdt.get());

    return gdtList;
}

bool ZoneCreator::CreateIgnoredAssetMap(const ZoneCreationContext& context, std::unordered_map<std::string, asset_type_t>& ignoredAssetMap) const
{
    for (const auto& ignoreEntry : context.m_ignored_assets.m_entries)
    {
        const auto foundAssetTypeEntry = m_asset_types_by_name.find(ignoreEntry.m_type);
        if (foundAssetTypeEntry == m_asset_types_by_name.end())
        {
            std::cout << "Unknown asset type \"" << ignoreEntry.m_type << "\" for ignore \"" << ignoreEntry.m_name << "\"" << std::endl;
            return false;
        }

        ignoredAssetMap[ignoreEntry.m_name] = foundAssetTypeEntry->second;
    }

    return true;
}

void ZoneCreator::CreateZoneAssetPools(Zone* zone) const
{
    zone->m_pools = std::make_unique<GameAssetPoolT6>(zone, zone->m_priority);

    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        zone->m_pools->InitPoolDynamic(assetType);
}

void ZoneCreator::HandleMetadata(Zone* zone, const ZoneCreationContext& context) const
{
    std::vector<KeyValuePair> kvpList;

    for (const auto& metaData : context.m_definition->m_metadata)
    {
        if (metaData->m_key.rfind("level.", 0) == 0)
        {
            const std::string strValue = metaData->m_key.substr(std::char_traits<char>::length("level."));
            if (strValue.empty())
                continue;

            int keyHash;
            if (strValue[0] == '@')
            {
                char* endPtr;
                keyHash = strtol(&strValue[1], &endPtr, 16);

                if (endPtr != &strValue[strValue.size()])
                {
                    std::cout << "Could not parse metadata key \"" << metaData->m_key << "\" as hash" << std::endl;
                    continue;
                }
            }
            else
            {
                keyHash = Common::Com_HashKey(strValue.c_str(), 64);
            }

            KeyValuePair kvp{keyHash, Common::Com_HashKey(zone->m_name.c_str(), 64), zone->GetMemory()->Dup(metaData->m_value.c_str())};
            kvpList.push_back(kvp);
        }
    }

    if (!kvpList.empty())
    {
        auto* kvps = zone->GetMemory()->Create<KeyValuePairs>();
        kvps->name = zone->GetMemory()->Dup(zone->m_name.c_str());
        kvps->numVariables = kvpList.size();
        kvps->keyValuePairs = static_cast<KeyValuePair*>(zone->GetMemory()->Alloc(sizeof(KeyValuePair) * kvpList.size()));

        for (auto i = 0u; i < kvpList.size(); i++)
            kvps->keyValuePairs[i] = kvpList[i];

        zone->m_pools->AddAsset(ASSET_TYPE_KEYVALUEPAIRS, zone->m_name, kvps, std::vector<XAssetInfoGeneric*>(), std::vector<scr_string_t>());
    }
}

bool ZoneCreator::SupportsGame(const std::string& gameName) const
{
    return gameName == g_GameT6.GetShortName();
}

std::unique_ptr<Zone> ZoneCreator::CreateZoneForDefinition(ZoneCreationContext& context) const
{
    auto zone = std::make_unique<Zone>(context.m_definition->m_name, 0, &g_GameT6);
    CreateZoneAssetPools(zone.get());

    for (const auto& assetEntry : context.m_definition->m_assets)
    {
        if (!assetEntry.m_is_reference)
            continue;

        context.m_ignored_assets.m_entries.emplace_back(assetEntry.m_asset_type, assetEntry.m_asset_name, assetEntry.m_is_reference);
    }

    const auto assetLoadingContext = std::make_unique<AssetLoadingContext>(zone.get(), context.m_asset_search_path, CreateGdtList(context));
    if (!CreateIgnoredAssetMap(context, assetLoadingContext->m_ignored_asset_map))
        return nullptr;

    HandleMetadata(zone.get(), context);

    for (const auto& assetEntry : context.m_definition->m_assets)
    {
        const auto foundAssetTypeEntry = m_asset_types_by_name.find(assetEntry.m_asset_type);
        if (foundAssetTypeEntry == m_asset_types_by_name.end())
        {
            std::cout << "Unknown asset type \"" << assetEntry.m_asset_type << "\"" << std::endl;
            return nullptr;
        }

        if (!ObjLoading::LoadAssetForZone(assetLoadingContext.get(), foundAssetTypeEntry->second, assetEntry.m_asset_name))
            return nullptr;
    }

    ObjLoading::FinalizeAssetsForZone(assetLoadingContext.get());

    return zone;
}
