#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "SearchPath/ISearchPath.h"
#include "Obj/Gdt/Gdt.h"
#include "Zone/AssetList/AssetList.h"
#include "Zone/Definition/ZoneDefinition.h"

class ZoneCreationContext
{
public:
    std::string m_game_name;
    ISearchPath* m_asset_search_path;
    ZoneDefinition* m_definition;
    std::vector<std::unique_ptr<Gdt>> m_gdt_files;
    std::vector<AssetListEntry> m_ignored_assets;

    ZoneCreationContext();
    ZoneCreationContext(ISearchPath* assetSearchPath, ZoneDefinition* definition);
};
