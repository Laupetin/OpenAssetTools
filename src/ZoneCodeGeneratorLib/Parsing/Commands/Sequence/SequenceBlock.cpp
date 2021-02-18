#include "SequenceBlock.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceBlock::SequenceBlock()
{
    const CommandsMatcherFactory create(this);

    AddMatchers({
        create.Keyword("block"),
        create.Identifier().Capture(CAPTURE_BLOCK_ENUM_ENTRY),
        create.Identifier().Capture(CAPTURE_BLOCK_TYPE),
        create.Optional(create.Keyword("default").Tag(TAG_DEFAULT)),
        create.Char(';')
    });
}

void SequenceBlock::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
}
