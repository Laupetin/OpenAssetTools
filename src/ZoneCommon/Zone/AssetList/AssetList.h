#pragma once

#include "Zone/ZoneTypes.h"

#include <string>
#include <vector>

class AssetListEntry
{
public:
    asset_type_t m_type;
    std::string m_name;
    bool m_is_reference;

    AssetListEntry();
    AssetListEntry(asset_type_t type, std::string name, bool isReference);
};

class AssetList
{
public:
    std::vector<AssetListEntry> m_entries;
};
