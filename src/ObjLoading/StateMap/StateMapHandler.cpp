#include "StateMapHandler.h"

#include <cassert>
#include <iostream>
#include <algorithm>

using namespace state_map;

StateMapHandler::StateMapHandler(const StateMapLayout& stateMapLayout, const StateMapDefinition& stateMap)
    : m_state_map_layout(stateMapLayout),
      m_state_map(stateMap)
{
}

void StateMapHandler::ApplyStateMap(const uint32_t* baseStateBits, uint32_t* outStateBits) const
{
    assert(baseStateBits != nullptr);
    assert(outStateBits != nullptr);

    for (auto i = 0u; i < m_state_map_layout.m_state_bits_count; i++)
        outStateBits[i] = baseStateBits[i];

    for (auto entryIndex = 0u; entryIndex < m_state_map.m_state_map_entries.size(); entryIndex++)
    {
        const auto& entry = m_state_map.m_state_map_entries[entryIndex];
        const auto matchingRule = std::find_if(entry.m_rules.begin(), entry.m_rules.end(), [&baseStateBits](const std::unique_ptr<StateMapRule>& rule)
        {
            const auto matchingCondition = std::find_if(rule->m_conditions.begin(), rule->m_conditions.end(), [&baseStateBits](const StateMapCondition& condition)
            {
                return ConditionIsTrue(condition, baseStateBits);
            });

            return matchingCondition != rule->m_conditions.end();
        });

        if (matchingRule != entry.m_rules.end())
            ApplyRule(m_state_map_layout.m_entry_layout.m_entries[entryIndex], **matchingRule, outStateBits);
        else
            ApplyRule(m_state_map_layout.m_entry_layout.m_entries[entryIndex], *entry.m_rules[entry.m_default_index], outStateBits);
    }
}

void StateMapHandler::ApplyRule(const StateMapLayoutEntry& entry, const StateMapRule& rule, uint32_t* outStateBits)
{
    if (rule.m_passthrough)
        return;

    outStateBits[entry.m_state_bits_index] &= ~entry.m_state_bits_mask;
    outStateBits[entry.m_state_bits_index] |= rule.m_value;
}

bool StateMapHandler::ConditionIsTrue(const StateMapCondition& condition, const uint32_t* stateBits)
{
    const auto stateBitCount = std::min(condition.m_masks_per_index.size(), condition.m_values_per_index.size());
    for (auto stateBitIndex = 0u; stateBitIndex < stateBitCount; stateBitIndex++)
    {
        if ((stateBits[stateBitIndex] & condition.m_masks_per_index[stateBitIndex]) != condition.m_values_per_index[stateBitIndex])
            return false;
    }

    return true;
}
