#include "AssetList.h"

AssetListEntry::AssetListEntry()
    : m_is_reference(false)
{
}

AssetListEntry::AssetListEntry(std::string type, std::string name, const bool isReference)
    : m_type(std::move(type)),
      m_name(std::move(name)),
      m_is_reference(isReference)
{
}
