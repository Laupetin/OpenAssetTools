#pragma once

#include "Techset/StateMap/StateMapDefinition.h"
#include "Techset/StateMap/StateMapLayout.h"

#include <cstdint>
#include <unordered_map>

namespace state_map
{
    class StateMapVars final : public ISimpleExpressionScopeValues
    {
    public:
        void AddValue(std::string key, std::string value);
        [[nodiscard]] SimpleExpressionValue ValueByName(const std::string& name) const override;

    private:
        std::unordered_map<std::string, std::string> m_vars;
    };

    class StateMapHandler
    {
    public:
        StateMapHandler(const StateMapLayout& stateMapLayout, const StateMapDefinition& stateMap);

        void ApplyStateMap(const uint32_t* baseStateBits, uint32_t* outStateBits) const;

    private:
        StateMapVars BuildVars(const uint32_t* baseStateBits) const;
        static void ApplyRule(const StateMapLayoutEntry& entry, const StateMapRule& rule, uint32_t* outStateBits);

        const StateMapLayout& m_state_map_layout;
        const StateMapDefinition& m_state_map;
    };
} // namespace state_map
