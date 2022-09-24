#pragma once

#include <cstdint>

#include "StateMap/StateMapDefinition.h"
#include "StateMap/StateMapLayout.h"

namespace state_map
{
    class StateMapHandler
    {
    public:
        StateMapHandler(const StateMapLayout& stateMapLayout, const StateMapDefinition& stateMap);

        void ApplyStateMap(const uint32_t* baseStateBits, uint32_t* outStateBits) const;

    private:
        static void ApplyRule(const StateMapLayoutEntry& entry, const StateMapRule& rule, uint32_t* outStateBits);
        static bool ConditionIsTrue(const StateMapCondition& condition, const uint32_t* stateBits);

        const StateMapLayout& m_state_map_layout;
        const StateMapDefinition& m_state_map;
    };
}
