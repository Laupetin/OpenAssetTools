#pragma once
#include <string>
#include <vector>
#include <memory>

#include "SearchPath/ISearchPath.h"
#include "Obj/Gdt/Gdt.h"

class AssetLoadingContext
{
public:
    ISearchPath* m_asset_search_path;
    std::string m_zone_name;
    std::string m_game_name;
    std::vector<std::unique_ptr<Gdt>> m_gdt_files;

    AssetLoadingContext();
    AssetLoadingContext(std::string zoneName, ISearchPath* assetSearchPath);
};
