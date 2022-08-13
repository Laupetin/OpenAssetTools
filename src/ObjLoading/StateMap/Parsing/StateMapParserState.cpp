#include "StateMapParserState.h"

using namespace state_map;

StateMapParserState::StateMapParserState(std::string stateMapName, const StateMapLayout& layout)
    : m_layout(layout),
      m_definition(std::make_unique<StateMapDefinition>(std::move(stateMapName), layout.m_layout_entries.size())),
      m_in_entry(false),
      m_current_entry_index(0u),
      m_current_rule(nullptr)
{
    for (auto i = 0u; i < m_layout.m_layout_entries.size(); i++)
        m_valid_state_map_entry_names.emplace(m_layout.m_layout_entries[i].m_name, i);
}
