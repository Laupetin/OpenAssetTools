#pragma once

#include <unordered_map>
#include <vector>

class ZoneDefinitionEntry
{
public:
    std::string m_asset_type;
    std::string m_asset_name;
    bool m_is_reference;

    ZoneDefinitionEntry();
    ZoneDefinitionEntry(std::string type, std::string name, bool isReference);
};

class ZoneDefinition
{
public:
    std::unordered_map<std::string, std::string> m_metadata;
    std::vector<std::string> m_includes;
    std::vector<std::string> m_ignores;
    std::vector<ZoneDefinitionEntry> m_assets;
};