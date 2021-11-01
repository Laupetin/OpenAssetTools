#pragma once

#include "AbstractPropertySequenceHolder.h"

namespace menu
{
    class MenuPropertySequences final : AbstractPropertySequenceHolder
    {
    public:
        MenuPropertySequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences);

        void AddSequences(FeatureLevel featureLevel) override;
    };
}
