#include "SequenceSetBlock.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceSetBlock::SequenceSetBlock()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("block"),
        create.Or({
            create.And({
                create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
                create.Identifier().Capture(CAPTURE_BLOCK_ENUM_ENTRY)
            }),
            create.Identifier().Capture(CAPTURE_BLOCK_ENUM_ENTRY)
        }),
        create.Char(';')
    });
}

void SequenceSetBlock::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
}
