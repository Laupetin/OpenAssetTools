#include "SequenceSetWordSize.h"

#include "Domain/Environment/WordSize.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

namespace
{
    static constexpr auto CAPTURE_WORD_SIZE = 1;
}

SequenceSetWordSize::SequenceSetWordSize()
{
    const CommandsMatcherFactory create(this);

    AddMatchers({
        create.Keyword("set"),
        create.Keyword("wordsize"),
        create
            .Or({
                create.Integer(),
                create.Keyword("native"),
            })
            .Capture(CAPTURE_WORD_SIZE),
        create.Char(';'),
    });
}

void SequenceSetWordSize::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& wordSizeToken = result.NextCapture(CAPTURE_WORD_SIZE);
    auto* structure = state->GetInUse();
    if (structure == nullptr)
        throw ParsingException(wordSizeToken.GetPos(), "No structure is in use");

    if (wordSizeToken.m_type == CommandsParserValueType::IDENTIFIER)
    {
        structure->m_word_size = OWN_WORD_SIZE;
        return;
    }

    switch (wordSizeToken.IntegerValue())
    {
    case 32:
        structure->m_word_size = WordSize::BITS_32;
        break;

    case 64:
        structure->m_word_size = WordSize::BITS_64;
        break;

    default:
        throw ParsingException(wordSizeToken.GetPos(), "Unknown word size");
    }
}
