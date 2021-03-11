#include "AssetList.h"

AssetListEntry::AssetListEntry()
= default;

AssetListEntry::AssetListEntry(std::string type, std::string name)
    : m_type(std::move(type)),
      m_name(std::move(name))
{
}
