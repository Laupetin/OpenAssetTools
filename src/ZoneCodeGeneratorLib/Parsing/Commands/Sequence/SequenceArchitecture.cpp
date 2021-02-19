#include "SequenceArchitecture.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceArchitecture::SequenceArchitecture()
{
    const CommandsMatcherFactory create(this);

    AddMatchers({
        create.Keyword("architecture"),
        create.Or({
            create.Keyword("x86").Tag(TAG_X86),
            create.Keyword("x64").Tag(TAG_X64)
        }),
        create.Char(';')
    });
}

void SequenceArchitecture::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    switch (result.NextTag())
    {
    case TAG_X86:
        state->SetArchitecture(Architecture::X86);
        break;

    case TAG_X64:
        state->SetArchitecture(Architecture::X64);
        break;

    default:
        throw ParsingException(TokenPos(), "Unknown architecture!");
    }
}
