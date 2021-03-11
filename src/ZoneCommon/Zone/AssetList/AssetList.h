#pragma once
#include <string>

class AssetListEntry
{
public:
    std::string m_type;
    std::string m_name;

    AssetListEntry();
    AssetListEntry(std::string type, std::string name);
};
