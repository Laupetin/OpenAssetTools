#pragma once

#include "AbstractScopeSequenceHolder.h"

namespace menu
{
    class FunctionScopeSequences final : AbstractScopeSequenceHolder
    {
    public:
        FunctionScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences);

        void AddSequences(FeatureLevel featureLevel) override;
    };
}
