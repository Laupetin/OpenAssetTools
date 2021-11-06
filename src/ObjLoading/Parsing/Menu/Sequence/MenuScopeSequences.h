#pragma once

#include "AbstractScopeSequenceHolder.h"

namespace menu
{
    class MenuScopeSequences final : AbstractScopeSequenceHolder
    {
    public:
        MenuScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences);

        void AddSequences(FeatureLevel featureLevel) override;
    };
}
