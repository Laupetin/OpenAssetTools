#pragma once
#include <string>
#include <vector>

class AssetListEntry
{
public:
    std::string m_type;
    std::string m_name;
    bool m_is_reference;

    AssetListEntry();
    AssetListEntry(std::string type, std::string name, bool isReference);
};

class AssetList
{
public:
    std::vector<AssetListEntry> m_entries;
};