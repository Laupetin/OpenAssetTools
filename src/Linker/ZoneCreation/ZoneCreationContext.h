#pragma once
#include "Obj/Gdt/Gdt.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/AssetList/AssetList.h"
#include "Zone/Definition/ZoneDefinition.h"

#include <memory>
#include <vector>

class ZoneCreationContext
{
public:
    ZoneDefinition* m_definition;
    ISearchPath* m_asset_search_path;
    std::vector<std::unique_ptr<Gdt>> m_gdt_files;
    AssetList m_ignored_assets;

    ZoneCreationContext();
    ZoneCreationContext(ZoneDefinition* definition, ISearchPath* assetSearchPath);
};
