#pragma once

#include "IGdtQueryable.h"
#include "Obj/Gdt/Gdt.h"

#include <string>
#include <unordered_map>
#include <vector>

class GdtLookup : public IGdtQueryable
{
public:
    void Initialize(const std::vector<const Gdt*>& gdtFiles);
    GdtEntry* GetGdtEntryByGdfAndName(const std::string& gdfName, const std::string& entryName) override;

private:
    std::unordered_map<std::string, std::unordered_map<std::string, GdtEntry*>> m_entries_by_gdf_and_by_name;
};
