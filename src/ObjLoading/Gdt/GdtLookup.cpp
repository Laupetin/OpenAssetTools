#include "GdtLookup.h"

void GdtLookup::Initialize(const std::vector<const Gdt*>& gdtFiles)
{
    m_entries_by_gdf_and_by_name.clear();

    for (const auto* gdt : gdtFiles)
    {
        for (const auto& entry : gdt->m_entries)
        {
            auto gdfMapEntry = m_entries_by_gdf_and_by_name.find(entry->m_gdf_name);
            if (gdfMapEntry == m_entries_by_gdf_and_by_name.end())
            {
                std::unordered_map<std::string, GdtEntry*> entryMap;
                entryMap.emplace(entry->m_name, entry.get());
                m_entries_by_gdf_and_by_name.emplace(entry->m_gdf_name, std::move(entryMap));
            }
            else
            {
                auto entryMapEntry = gdfMapEntry->second.find(entry->m_name);

                if (entryMapEntry == gdfMapEntry->second.end())
                    gdfMapEntry->second.emplace(entry->m_name, entry.get());
                else
                    entryMapEntry->second = entry.get();
            }
        }
    }
}

GdtEntry* GdtLookup::GetGdtEntryByGdfAndName(const std::string& gdfName, const std::string& entryName)
{
    const auto foundGdtMap = m_entries_by_gdf_and_by_name.find(gdfName);

    if (foundGdtMap == m_entries_by_gdf_and_by_name.end())
        return nullptr;

    const auto foundGdtEntry = foundGdtMap->second.find(entryName);

    if (foundGdtEntry == foundGdtMap->second.end())
        return nullptr;

    return foundGdtEntry->second;
}
