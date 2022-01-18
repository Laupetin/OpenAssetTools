#pragma once

#include "Parsing/Sequence/AbstractScopeSequenceHolder.h"
#include "Parsing/StructuredDataDef/StructuredDataDefParser.h"

namespace sdd
{
    class StructuredDataDefEnumScopeSequences final : AbstractScopeSequenceHolder<StructuredDataDefParser>
    {
    public:
        StructuredDataDefEnumScopeSequences(std::vector<std::unique_ptr<StructuredDataDefParser::sequence_t>>& allSequences, std::vector<StructuredDataDefParser::sequence_t*>& scopeSequences);

        void AddSequences() const;
    };
}
