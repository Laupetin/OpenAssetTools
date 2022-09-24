#include "StateMapDefinition.h"

using namespace state_map;

StateMapCondition::StateMapCondition(const size_t entryCount)
    : m_masks_per_index(entryCount),
      m_values_per_index(entryCount)
{
}

StateMapRule::StateMapRule()
    : m_value(0u),
      m_passthrough(false)
{
}

StateMapEntry::StateMapEntry()
    : m_default_index(0u)
{
}

StateMapDefinition::StateMapDefinition(std::string name, const size_t entryCount)
    : m_name(std::move(name)),
      m_state_map_entries(entryCount)
{
}
