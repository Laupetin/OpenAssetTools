#pragma once
#include <string>
#include <vector>

class AssetListEntry
{
public:
    std::string m_type;
    std::string m_name;

    AssetListEntry();
    AssetListEntry(std::string type, std::string name);
};

class AssetList
{
public:
    std::vector<AssetListEntry> m_entries;
};