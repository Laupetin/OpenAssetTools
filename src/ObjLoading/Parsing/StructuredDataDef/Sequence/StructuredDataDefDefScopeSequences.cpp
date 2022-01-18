#include "StructuredDataDefDefScopeSequences.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

namespace sdd::def_scope_sequences
{
    class SequenceEnum final : public StructuredDataDefParser::sequence_t
    {
        static constexpr auto CAPTURE_NAME = 1;

    public:
        SequenceEnum()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Keyword("enum"),
                create.Identifier().Capture(CAPTURE_NAME),
                create.Char('{')
            });
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_def);

            auto newEnum = std::make_unique<CommonStructuredDataDefEnum>(result.NextCapture(CAPTURE_NAME).IdentifierValue());
            state->m_current_enum = newEnum.get();
            state->m_current_def->m_enums.emplace_back(std::move(newEnum));
        }
    };

    class SequenceStruct final : public StructuredDataDefParser::sequence_t
    {
        static constexpr auto CAPTURE_NAME = 1;

    public:
        SequenceStruct()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Keyword("struct"),
                create.Identifier().Capture(CAPTURE_NAME),
                create.Char('{')
            });
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_def);

            auto newStruct = std::make_unique<CommonStructuredDataDefStruct>(result.NextCapture(CAPTURE_NAME).IdentifierValue());
            state->m_current_struct = newStruct.get();
            state->m_current_def->m_structs.emplace_back(std::move(newStruct));
        }
    };

    class SequenceCloseScope final : public StructuredDataDefParser::sequence_t
    {
        static constexpr auto CAPTURE_NAME = 1;

    public:
        SequenceCloseScope()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Char('}')
            });
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_enum == nullptr);
            assert(state->m_current_struct == nullptr);

            state->m_current_def = nullptr;
        }
    };
}

using namespace sdd;
using namespace def_scope_sequences;

StructuredDataDefDefScopeSequences::StructuredDataDefDefScopeSequences(std::vector<std::unique_ptr<StructuredDataDefParser::sequence_t>>& allSequences,
                                                                       std::vector<StructuredDataDefParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void StructuredDataDefDefScopeSequences::AddSequences() const
{
    AddSequence(std::make_unique<SequenceEnum>());
    AddSequence(std::make_unique<SequenceStruct>());
}
