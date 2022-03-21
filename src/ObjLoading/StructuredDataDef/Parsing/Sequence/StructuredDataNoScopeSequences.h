#pragma once

#include "Parsing/Sequence/AbstractScopeSequenceHolder.h"
#include "StructuredDataDef/Parsing/StructuredDataDefParser.h"

namespace sdd
{
    class StructuredDataNoScopeSequences final : AbstractScopeSequenceHolder<StructuredDataDefParser>
    {
    public:
        StructuredDataNoScopeSequences(std::vector<std::unique_ptr<StructuredDataDefParser::sequence_t>>& allSequences, std::vector<StructuredDataDefParser::sequence_t*>& scopeSequences);

        void AddSequences() const;
    };
}
