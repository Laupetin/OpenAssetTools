#include "SequenceSetAliasWordSize.h"

#include "Domain/Environment/WordSize.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

namespace
{
    static constexpr auto CAPTURE_WORD_SIZE = 1;
}

SequenceSetAliasWordSize::SequenceSetAliasWordSize()
{
    const CommandsMatcherFactory create(this);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("aliaswordsize"),
        create.Or({create.Integer(), create.Keyword("native")}).Capture(CAPTURE_WORD_SIZE),
        create.Char(';'),
    });
}

void SequenceSetAliasWordSize::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& token = result.NextCapture(CAPTURE_WORD_SIZE);
    auto* structure = state->GetInUse();
    if (structure == nullptr)
        throw ParsingException(token.GetPos(), "No structure is in use");

    if (token.m_type == CommandsParserValueType::IDENTIFIER)
    {
        structure->m_alias_word_size = OWN_WORD_SIZE;
        return;
    }

    switch (token.IntegerValue())
    {
    case 32:
        structure->m_alias_word_size = WordSize::BITS_32;
        break;
    case 64:
        structure->m_alias_word_size = WordSize::BITS_64;
        break;
    default:
        throw ParsingException(token.GetPos(), "Unknown alias word size");
    }
}
