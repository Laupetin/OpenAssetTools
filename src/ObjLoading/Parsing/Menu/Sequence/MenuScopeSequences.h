#pragma once

#include "Parsing/Menu/MenuFileParser.h"
#include "Parsing/Sequence/AbstractScopeSequenceHolder.h"

namespace menu
{
    class MenuScopeSequences final : AbstractScopeSequenceHolder<MenuFileParser>
    {
    public:
        MenuScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences);

        void AddSequences(FeatureLevel featureLevel, bool permissive) const;
    };
} // namespace menu
