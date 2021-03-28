#include "AssetLoadingContext.h"

AssetLoadingContext::AssetLoadingContext(Zone* zone, ISearchPath* rawSearchPath, std::vector<Gdt*> gdtFiles)
    : m_zone(zone),
      m_raw_search_path(rawSearchPath),
      m_gdt_files(std::move(gdtFiles))
{
    BuildGdtEntryCache();
}

void AssetLoadingContext::BuildGdtEntryCache()
{
    for (auto* gdt : m_gdt_files)
    {
        for (const auto& entry : gdt->m_entries)
        {
            auto gdfMapEntry = m_entries_by_gdf_and_by_name.find(entry->m_gdf_name);
            if (gdfMapEntry == m_entries_by_gdf_and_by_name.end())
            {
                std::unordered_map<std::string, GdtEntry*> entryMap;
                entryMap.emplace(std::make_pair(entry->m_name, entry.get()));
                m_entries_by_gdf_and_by_name.emplace(std::make_pair(entry->m_gdf_name, std::move(entryMap)));
            }
            else
            {
                auto entryMapEntry = gdfMapEntry->second.find(entry->m_name);

                if (entryMapEntry == gdfMapEntry->second.end())
                    gdfMapEntry->second.emplace(std::make_pair(entry->m_name, entry.get()));
                else
                    entryMapEntry->second = entry.get();
            }
        }
    }
}

GdtEntry* AssetLoadingContext::GetGdtEntryByGdfAndName(const std::string& gdfName, const std::string& entryName)
{
    const auto foundGdtMap = m_entries_by_gdf_and_by_name.find(gdfName);

    if (foundGdtMap == m_entries_by_gdf_and_by_name.end())
        return nullptr;

    const auto foundGdtEntry = foundGdtMap->second.find(entryName);

    if (foundGdtEntry == foundGdtMap->second.end())
        return nullptr;

    return foundGdtEntry->second;
}
