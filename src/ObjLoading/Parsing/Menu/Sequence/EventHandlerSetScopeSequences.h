#pragma once

#include "AbstractScopeSequenceHolder.h"

namespace menu
{
    class EventHandlerSetScopeSequences final : AbstractScopeSequenceHolder
    {
    public:
        EventHandlerSetScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences);

        void AddSequences(FeatureLevel featureLevel, bool permissive) override;
    };
}
