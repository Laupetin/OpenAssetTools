#include "StateMapDefinition.h"

using namespace state_map;

bool StateMapRule::IsPassthrough() const
{
    return m_values.empty();
}

StateMapDefinition::StateMapDefinition(const size_t entryCount)
    : m_state_map_entries(entryCount)
{
}
