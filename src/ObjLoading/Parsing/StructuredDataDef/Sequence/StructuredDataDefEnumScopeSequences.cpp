#include "StructuredDataDefEnumScopeSequences.h"

#include <algorithm>

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

namespace sdd::enum_scope_sequences
{
    class SequenceEnumEntry final : public StructuredDataDefParser::sequence_t
    {
        static constexpr auto CAPTURE_ENTRY_VALUE = 1;

    public:
        SequenceEnumEntry()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.String().Capture(CAPTURE_ENTRY_VALUE),
                create.Or({
                    create.Char(','),
                    create.Char('}').NoConsume()
                })
            });
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_enum);

            state->m_current_enum->m_entries.emplace_back(result.NextCapture(CAPTURE_ENTRY_VALUE).StringValue(), state->m_current_enum->m_entries.size());
        }
    };

    class SequenceCloseEnum final : public StructuredDataDefParser::sequence_t
    {
    public:
        SequenceCloseEnum()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Char('}'),
                create.Optional(create.Char(';'))
            });
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_enum != nullptr);

            std::sort(state->m_current_enum->m_entries.begin(), state->m_current_enum->m_entries.end(), [](const CommonStructuredDataDefEnumEntry& e1, const CommonStructuredDataDefEnumEntry& e2)
            {
                return e1.m_name < e2.m_name;
            });

            state->m_current_enum = nullptr;
        }
    };
}

using namespace sdd;
using namespace enum_scope_sequences;

StructuredDataDefEnumScopeSequences::StructuredDataDefEnumScopeSequences(std::vector<std::unique_ptr<StructuredDataDefParser::sequence_t>>& allSequences,
                                                                         std::vector<StructuredDataDefParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void StructuredDataDefEnumScopeSequences::AddSequences() const
{
    AddSequence(std::make_unique<SequenceCloseEnum>());
    AddSequence(std::make_unique<SequenceEnumEntry>());
}
