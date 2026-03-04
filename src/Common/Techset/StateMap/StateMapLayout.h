#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace state_map
{
    class StateMapLayoutEntry
    {
    public:
        StateMapLayoutEntry(std::string name, size_t stateBitsIndex, size_t stateBitsMask, std::vector<std::string> resultVars);

        std::string m_name;
        size_t m_state_bits_index;
        size_t m_state_bits_mask;
        std::vector<std::string> m_result_vars;
    };

    class StateMapLayoutEntries
    {
    public:
        explicit StateMapLayoutEntries(std::vector<StateMapLayoutEntry> entryList);

        std::vector<StateMapLayoutEntry> m_entries;
    };

    class StateMapLayoutVarValue
    {
    public:
        StateMapLayoutVarValue(std::string name, size_t stateBitsMask);

        std::string m_name;
        size_t m_state_bits_mask;
    };

    class StateMapLayoutVar
    {
    public:
        StateMapLayoutVar(std::string name, size_t stateBitsIndex, std::vector<StateMapLayoutVarValue> values);

        std::string m_name;
        size_t m_state_bits_index;
        std::vector<StateMapLayoutVarValue> m_values;
    };

    class StateMapLayoutVars
    {
    public:
        explicit StateMapLayoutVars(std::vector<StateMapLayoutVar> varList);

        std::vector<StateMapLayoutVar> m_vars;
    };

    class StateMapLayout
    {
    public:
        StateMapLayout(size_t stateBitsCount, const StateMapLayoutEntries& entryLayout, const StateMapLayoutVars& varLayout);

        size_t m_state_bits_count;
        const StateMapLayoutEntries& m_entry_layout;
        const StateMapLayoutVars& m_var_layout;
    };
} // namespace state_map
