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
        std::vector<std::unique_ptr<StateMapRule>> m_rules;
    };

    class StateMapDefinition
    {
    public:
        explicit StateMapDefinition(size_t entryCount);

        std::vector<StateMapEntry> m_state_map_entries;
    };
}
