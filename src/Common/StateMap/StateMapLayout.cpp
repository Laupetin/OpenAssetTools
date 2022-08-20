#include "StateMapLayout.h"

#include <cassert>

using namespace state_map;

namespace state_map
{
#ifdef _DEBUG
    void ValidateStateMapLayout(const StateMapLayout& layout)
    {
        // Must have at least one state bits field
        assert(layout.m_state_bits_count > 0);

        for (const auto& var : layout.m_var_layout.m_vars)
        {
            // Cannot exceed state bits fields
            assert(var.m_state_bits_index < layout.m_state_bits_count);

            for (auto i = 0u; i < var.m_values.size(); i++)
            {
                const auto& value = var.m_values[i];

                // Zero mask values must be last
                assert(value.m_state_bits_mask != 0 || i == var.m_values.size() - 1);
            }
        }

        for (const auto& entry : layout.m_entry_layout.m_entries)
        {
            // Cannot exceed state bits fields
            assert(entry.m_state_bits_index < layout.m_state_bits_count);

            // Has to have a mask for passthrough
            assert(entry.m_state_bits_mask != 0);

            for (auto& resultVar : entry.m_result_vars)
            {
                const auto correspondingVar = std::find_if(layout.m_var_layout.m_vars.begin(), layout.m_var_layout.m_vars.end(), [&resultVar](const StateMapLayoutVar& var)
                {
                    return var.m_name == resultVar;
                });

                // Has to have a corresponding var
                assert(correspondingVar != layout.m_var_layout.m_vars.end());
            }
        }
    }
#endif
}

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
#ifdef _DEBUG
    ValidateStateMapLayout(*this);
#endif
}
