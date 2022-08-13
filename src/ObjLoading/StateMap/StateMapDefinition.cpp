#include "StateMapDefinition.h"

using namespace state_map;

bool StateMapRule::IsPassthrough() const
{
    return m_values.empty();
}

StateMapEntry::StateMapEntry()
    : m_has_default(false),
      m_default_index(0u)
{
}

StateMapDefinition::StateMapDefinition(std::string name, const size_t entryCount)
    : m_name(std::move(name)),
      m_state_map_entries(entryCount)
{
}
