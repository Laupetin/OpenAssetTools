#pragma once

#include <map>
#include <string>

class GdtEntry
{
public:
    std::string m_name;
    std::string m_gdf_name;
    GdtEntry* m_parent;
    std::map<std::string, std::string> m_properties;

    GdtEntry();
    GdtEntry(std::string name, std::string gdfName);
    GdtEntry(std::string name, GdtEntry* parent);
};
