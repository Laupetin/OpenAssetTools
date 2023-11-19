#pragma once
#include "Obj/Gdt/Gdt.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/AssetList/AssetList.h"
#include "Zone/Definition/ZoneDefinition.h"

#include <memory>
#include <string>
#include <vector>

class ZoneCreationContext
{
public:
    std::string m_game_name;
    ISearchPath* m_asset_search_path;
    ZoneDefinition* m_definition;
    std::vector<std::unique_ptr<Gdt>> m_gdt_files;
    AssetList m_ignored_assets;

    ZoneCreationContext();
    ZoneCreationContext(ISearchPath* assetSearchPath, ZoneDefinition* definition);
};
