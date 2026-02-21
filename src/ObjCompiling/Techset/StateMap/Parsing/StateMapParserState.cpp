#include "StateMapParserState.h"

using namespace state_map;

StateMapParserState::StateMapParserState(std::string stateMapName, const StateMapLayout& layout)
    : m_layout(layout),
      m_definition(std::make_unique<StateMapDefinition>(std::move(stateMapName), layout.m_entry_layout.m_entries.size())),
      m_in_entry(false),
      m_entry_has_default(false),
      m_current_entry_index(0u),
      m_current_rule(nullptr)
{
    for (auto i = 0u; i < m_layout.m_entry_layout.m_entries.size(); i++)
        m_valid_state_map_entry_names.emplace(m_layout.m_entry_layout.m_entries[i].m_name, i);

    for (auto varIndex = 0u; varIndex < layout.m_var_layout.m_vars.size(); varIndex++)
    {
        const auto& var = layout.m_var_layout.m_vars[varIndex];
        m_valid_vars.emplace(std::make_pair(var.m_name, varIndex));

        for (const auto& value : var.m_values)
            m_valid_values.emplace(value.m_name);
    }
}
