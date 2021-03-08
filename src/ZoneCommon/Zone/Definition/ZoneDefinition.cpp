#include "ZoneDefinition.h"

ZoneDefinitionEntry::ZoneDefinitionEntry()
    : m_is_reference(false)
{
}

ZoneDefinitionEntry::ZoneDefinitionEntry(std::string type, std::string name, bool isReference)
    : m_asset_type(std::move(type)),
      m_asset_name(std::move(name)),
      m_is_reference(isReference)
{
}
