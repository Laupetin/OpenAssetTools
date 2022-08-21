#include "StateMapHandler.h"

#include <cassert>
#include <iostream>
#include <algorithm>

using namespace state_map;

void StateMapVars::AddValue(std::string key, std::string value)
{
    m_vars.emplace(std::make_pair(std::move(key), std::move(value)));
}

SimpleExpressionValue StateMapVars::ValueByName(const std::string& name) const
{
    const auto foundValue = m_vars.find(name);

    if (foundValue != m_vars.end())
        return SimpleExpressionValue(foundValue->second);

    return SimpleExpressionValue(0);
}

StateMapHandler::StateMapHandler(const StateMapLayout& stateMapLayout, const StateMapDefinition& stateMap)
    : m_state_map_layout(stateMapLayout),
      m_state_map(stateMap)
{
}

void StateMapHandler::ApplyStateMap(const uint32_t* baseStateBits, uint32_t* outStateBits) const
{
    assert(baseStateBits != nullptr);
    assert(outStateBits != nullptr);

    const auto vars = BuildVars(baseStateBits);

    for (auto i = 0u; i < m_state_map_layout.m_state_bits_count; i++)
        outStateBits[i] = baseStateBits[i];

    for (auto entryIndex = 0u; entryIndex < m_state_map.m_state_map_entries.size(); entryIndex++)
    {
        const auto& entry = m_state_map.m_state_map_entries[entryIndex];
        const auto matchingRule = std::find_if(entry.m_rules.begin(), entry.m_rules.end(), [&vars](const std::unique_ptr<StateMapRule>& rule)
        {
            const auto matchingCondition = std::find_if(rule->m_conditions.begin(), rule->m_conditions.end(), [&vars](std::unique_ptr<ISimpleExpression>& condition)
            {
                return condition->EvaluateNonStatic(&vars).IsTruthy();
            });

            return matchingCondition != rule->m_conditions.end();
        });

        if (matchingRule != entry.m_rules.end())
            ApplyRule(m_state_map_layout.m_entry_layout.m_entries[entryIndex], **matchingRule, outStateBits);
        else
            ApplyRule(m_state_map_layout.m_entry_layout.m_entries[entryIndex], *entry.m_rules[entry.m_default_index], outStateBits);
    }
}

StateMapVars StateMapHandler::BuildVars(const uint32_t* baseStateBits) const
{
    StateMapVars result;

    for (const auto& var : m_state_map_layout.m_var_layout.m_vars)
    {
        const auto baseStateBitField = baseStateBits[var.m_state_bits_index];
        const auto matchingValue = std::find_if(var.m_values.begin(), var.m_values.end(), [&baseStateBitField](const StateMapLayoutVarValue& value)
        {
            return (baseStateBitField & value.m_state_bits_mask) == value.m_state_bits_mask;
        });

        if (matchingValue != var.m_values.end())
            result.AddValue(var.m_name, matchingValue->m_name);
        else
            std::cerr << "Could not find base value for state map var \"" << var.m_name << "\"\n";
    }

    return result;
}

void StateMapHandler::ApplyRule(const StateMapLayoutEntry& entry, const StateMapRule& rule, uint32_t* outStateBits)
{
    if (rule.m_passthrough)
        return;

    outStateBits[entry.m_state_bits_index] &= ~entry.m_state_bits_mask;
    outStateBits[entry.m_state_bits_index] |= rule.m_value;
}
