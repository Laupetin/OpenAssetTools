#pragma once

#include "Techset/StateMap/StateMapDefinition.h"
#include "Techset/StateMap/StateMapLayout.h"

#include <memory>
#include <set>
#include <string>
#include <unordered_map>

namespace state_map
{
    class StateMapParserState
    {
    public:
        const StateMapLayout& m_layout;
        std::unordered_map<std::string, size_t> m_valid_state_map_entry_names;
        std::unordered_map<std::string, size_t> m_valid_vars;
        std::set<std::string> m_valid_values;
        std::unique_ptr<StateMapDefinition> m_definition;

        bool m_in_entry;
        bool m_entry_has_default;
        size_t m_current_entry_index;
        StateMapRule* m_current_rule;

        StateMapParserState(std::string stateMapName, const StateMapLayout& layout);
    };
} // namespace state_map
