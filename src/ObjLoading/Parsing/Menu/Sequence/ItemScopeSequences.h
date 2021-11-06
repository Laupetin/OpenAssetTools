#pragma once

#include "AbstractScopeSequenceHolder.h"

namespace menu
{
    class ItemScopeSequences final : AbstractScopeSequenceHolder
    {
    public:
        ItemScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences);

        void AddSequences(FeatureLevel featureLevel) override;
    };
}
