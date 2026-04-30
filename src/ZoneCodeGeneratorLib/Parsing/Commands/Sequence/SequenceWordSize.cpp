#include "SequenceWordSize.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

namespace
{
    static constexpr auto CAPTURE_WORD_SIZE = 1;
}

SequenceWordSize::SequenceWordSize()
{
    const CommandsMatcherFactory create(this);

    AddMatchers({
        create.Keyword("wordsize"),
        create.Integer().Capture(CAPTURE_WORD_SIZE),
        create.Char(';'),
    });
}

void SequenceWordSize::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& wordSizeToken = result.NextCapture(CAPTURE_WORD_SIZE);

    switch (wordSizeToken.IntegerValue())
    {
    case 32:
        state->SetWordSize(WordSize::BITS_32);
        break;

    case 64:
        state->SetWordSize(WordSize::BITS_64);
        break;

    default:
        throw ParsingException(wordSizeToken.GetPos(), "Unknown word size");
    }
}
