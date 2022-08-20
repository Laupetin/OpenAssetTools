#include "StateMapLayout.h"

using namespace state_map;


StateMapLayoutEntry::StateMapLayoutEntry(std::string name, const size_t stateBitsIndex, const size_t stateBitsMask, std::vector<std::string> resultVars)
    : m_name(std::move(name)),
      m_state_bits_index(stateBitsIndex),
      m_state_bits_mask(stateBitsMask),
      m_result_vars(std::move(resultVars))
{
}

StateMapLayoutEntries::StateMapLayoutEntries(std::vector<StateMapLayoutEntry> entryList)
    : m_entries(std::move(entryList))
{
}

StateMapLayoutVarValue::StateMapLayoutVarValue(std::string name, const size_t stateBitsMask)
    : m_name(std::move(name)),
      m_state_bits_mask(stateBitsMask)
{
}

StateMapLayoutVar::StateMapLayoutVar(std::string name, const size_t stateBitsIndex, std::vector<StateMapLayoutVarValue> values)
    : m_name(std::move(name)),
      m_state_bits_index(stateBitsIndex),
      m_values(std::move(values))
{
}

StateMapLayoutVars::StateMapLayoutVars(std::vector<StateMapLayoutVar> varList)
    : m_vars(std::move(varList))
{
}

StateMapLayout::StateMapLayout(const size_t stateBitsCount, const StateMapLayoutEntries& entryLayout, const StateMapLayoutVars& varLayout)
    : m_state_bits_count(stateBitsCount),
      m_entry_layout(entryLayout),
      m_var_layout(varLayout)
{
}
