#pragma once

#include "Parsing/Menu/MenuFileParser.h"
#include "Parsing/Sequence/AbstractScopeSequenceHolder.h"

namespace menu
{
    class ItemScopeSequences final : AbstractScopeSequenceHolder<MenuFileParser>
    {
    public:
        ItemScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences);

        void AddSequences(FeatureLevel featureLevel, bool permissive) const;
    };
}
