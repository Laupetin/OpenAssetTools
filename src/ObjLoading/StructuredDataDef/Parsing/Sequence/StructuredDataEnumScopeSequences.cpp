#include "StructuredDataEnumScopeSequences.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

#include <algorithm>

namespace sdd::enum_scope_sequences
{
    class SequenceEnumEntry final : public StructuredDataDefParser::sequence_t
    {
        static constexpr auto CAPTURE_ENTRY_VALUE = 1;

    public:
        SequenceEnumEntry()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({create.String().Capture(CAPTURE_ENTRY_VALUE), create.Or({create.Char(','), create.Char('}').NoConsume()})});
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_enum);

            const auto& entryValueToken = result.NextCapture(CAPTURE_ENTRY_VALUE);
            if (state->m_current_enum->m_reserved_entry_count > 0
                && static_cast<size_t>(state->m_current_enum->m_reserved_entry_count) <= state->m_current_enum->m_entries.size())
                throw ParsingException(entryValueToken.GetPos(), "Enum entry count exceeds reserved count");

            state->m_current_enum->m_entries.emplace_back(entryValueToken.StringValue(), state->m_current_enum->m_entries.size());
        }
    };

    class SequenceCloseEnum final : public StructuredDataDefParser::sequence_t
    {
    public:
        SequenceCloseEnum()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({create.Char('}'), create.Optional(create.Char(';'))});
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_enum != nullptr);
            state->m_current_enum = nullptr;
        }
    };
} // namespace sdd::enum_scope_sequences

using namespace sdd;
using namespace enum_scope_sequences;

StructuredDataEnumScopeSequences::StructuredDataEnumScopeSequences(std::vector<std::unique_ptr<StructuredDataDefParser::sequence_t>>& allSequences,
                                                                   std::vector<StructuredDataDefParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void StructuredDataEnumScopeSequences::AddSequences() const
{
    AddSequence(std::make_unique<SequenceCloseEnum>());
    AddSequence(std::make_unique<SequenceEnumEntry>());
}
