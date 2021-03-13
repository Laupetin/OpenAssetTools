#pragma once
#include <string>
#include <vector>
#include <memory>

#include "SearchPath/ISearchPath.h"
#include "Obj/Gdt/Gdt.h"
#include "Zone/Definition/ZoneDefinition.h"

class ZoneCreationContext
{
public:
    std::string m_zone_name;
    std::string m_game_name;
    ISearchPath* m_asset_search_path;
    ZoneDefinition* m_definition;
    std::vector<std::unique_ptr<Gdt>> m_gdt_files;

    ZoneCreationContext();
    ZoneCreationContext(std::string zoneName, ISearchPath* assetSearchPath, ZoneDefinition* definition);
};
