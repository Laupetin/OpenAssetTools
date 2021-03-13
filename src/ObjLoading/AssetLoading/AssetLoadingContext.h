#pragma once

#include <unordered_map>

#include "IGdtQueryable.h"
#include "Obj/Gdt/Gdt.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/Zone.h"

class AssetLoadingContext final : public IGdtQueryable
{
    std::unordered_map<std::string, std::unordered_map<std::string, GdtEntry*>> m_entries_by_gdf_and_by_name;

    void BuildGdtEntryCache();

public:
    Zone* const m_zone;
    ISearchPath* const m_raw_search_path;
    const std::vector<Gdt*> m_gdt_files;

    AssetLoadingContext(Zone* zone, ISearchPath* rawSearchPath, std::vector<Gdt*> gdtFiles);
    GdtEntry* GetGdtEntryByGdfAndName(const std::string& gdfName, const std::string& entryName) override;
};
