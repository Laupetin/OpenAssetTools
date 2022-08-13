#pragma once

#include <string>
#include <vector>

namespace state_map
{
    class StateMapLayoutEntry
    {
    public:
        StateMapLayoutEntry(std::string name, size_t resultCount);

        std::string m_name;
        size_t m_result_count;
    };

    class StateMapLayout
    {
    public:
        explicit StateMapLayout(std::vector<StateMapLayoutEntry> layoutEntries);

        std::vector<StateMapLayoutEntry> m_layout_entries;
    };
}
