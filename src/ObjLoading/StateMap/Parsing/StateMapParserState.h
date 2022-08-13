#pragma once

#include <map>
#include <memory>
#include <string>

#include "StateMap/StateMapDefinition.h"
#include "StateMap/StateMapLayout.h"

namespace state_map
{
    class StateMapParserState
    {
    public:
        const StateMapLayout& m_layout;
        std::map<std::string, size_t> m_valid_state_map_entry_names;
        std::unique_ptr<StateMapDefinition> m_definition;

        bool m_in_entry;
        size_t m_current_entry_index;
        StateMapRule* m_current_rule;

        explicit StateMapParserState(const StateMapLayout& layout);
    };
}
