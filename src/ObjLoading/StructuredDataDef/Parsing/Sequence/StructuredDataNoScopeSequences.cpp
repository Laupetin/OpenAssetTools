#include "StructuredDataNoScopeSequences.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

namespace sdd::no_scope_sequences
{
    class SequenceVersion final : public StructuredDataDefParser::sequence_t
    {
        static constexpr auto CAPTURE_VERSION = 1;

    public:
        SequenceVersion()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Keyword("version"),
                create.Integer().Capture(CAPTURE_VERSION),
                create.Char('{'),
            });
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            auto newDef = std::make_unique<CommonStructuredDataDef>(result.NextCapture(CAPTURE_VERSION).IntegerValue());
            state->m_current_def = newDef.get();
            state->m_defs.emplace_back(std::move(newDef));
        }
    };
} // namespace sdd::no_scope_sequences

using namespace sdd;
using namespace no_scope_sequences;

StructuredDataNoScopeSequences::StructuredDataNoScopeSequences(std::vector<std::unique_ptr<StructuredDataDefParser::sequence_t>>& allSequences,
                                                               std::vector<StructuredDataDefParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void StructuredDataNoScopeSequences::AddSequences() const
{
    AddSequence(std::make_unique<SequenceVersion>());
}
