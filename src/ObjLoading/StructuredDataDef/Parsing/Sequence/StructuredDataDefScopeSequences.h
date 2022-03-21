#pragma once

#include "Parsing/Sequence/AbstractScopeSequenceHolder.h"
#include "StructuredDataDef/Parsing/StructuredDataDefParser.h"

namespace sdd
{
    class StructuredDataDefScopeSequences final : AbstractScopeSequenceHolder<StructuredDataDefParser>
    {
    public:
        StructuredDataDefScopeSequences(std::vector<std::unique_ptr<StructuredDataDefParser::sequence_t>>& allSequences, std::vector<StructuredDataDefParser::sequence_t*>& scopeSequences);

        void AddSequences() const;
    };
}
