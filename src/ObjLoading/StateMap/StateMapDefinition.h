#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Utils/ClassUtils.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"

namespace state_map
{
    class StateMapRule
    {
    public:
        std::vector<std::unique_ptr<ISimpleExpression>> m_conditions;
        std::vector<std::string> m_values;
        
        _NODISCARD bool IsPassthrough() const;
    };

    class StateMapEntry
    {
    public:
        bool m_has_default;
        size_t m_default_index;
        std::vector<std::unique_ptr<StateMapRule>> m_rules;

        StateMapEntry();
    };

    class StateMapDefinition
    {
    public:
        StateMapDefinition(std::string name, size_t entryCount);

        std::string m_name;
        std::vector<StateMapEntry> m_state_map_entries;
    };
}
