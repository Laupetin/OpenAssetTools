#include "StateMapLayout.h"

using namespace state_map;

StateMapLayoutEntry::StateMapLayoutEntry(std::string name, const size_t resultCount)
    : m_name(std::move(name)),
      m_result_count(resultCount)
{
}

StateMapLayout::StateMapLayout(std::vector<StateMapLayoutEntry> layoutEntries)
    : m_layout_entries(std::move(layoutEntries))
{
}
