#pragma once
#include <string>
#include <vector>
#include <memory>

#include "SearchPath/ISearchPath.h"
#include "Obj/Gdt/Gdt.h"

class ZoneCreationContext
{
public:
    ISearchPath* m_asset_search_path;
    std::string m_zone_name;
    std::string m_game_name;
    std::vector<std::unique_ptr<Gdt>> m_gdt_files;

    ZoneCreationContext();
    ZoneCreationContext(std::string zoneName, ISearchPath* assetSearchPath);
};
