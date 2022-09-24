#pragma once

#include <memory>
#include <string>
#include <vector>

namespace state_map
{
    class StateMapCondition
    {
    public:
        std::vector<size_t> m_masks_per_index;
        std::vector<size_t> m_values_per_index;

        explicit StateMapCondition(size_t entryCount);
    };

    class StateMapRule
    {
    public:
        std::vector<StateMapCondition> m_conditions;
        size_t m_value;
        bool m_passthrough;

        StateMapRule();
    };

    class StateMapEntry
    {
    public:
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
