#include "AssetList.h"

AssetListEntry::AssetListEntry()
    : m_type(0u),
      m_is_reference(false)
{
}

AssetListEntry::AssetListEntry(const asset_type_t type, std::string name, const bool isReference)
    : m_type(type),
      m_name(std::move(name)),
      m_is_reference(isReference)
{
}
