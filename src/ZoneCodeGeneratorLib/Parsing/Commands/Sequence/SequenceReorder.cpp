#include "SequenceReorder.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceReorder::SequenceReorder()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("reorder"),
        create.Optional(create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE)),
        create.Char(':'),
        create.Optional(create.And({
            create.Char('.'),
            create.Char('.'),
            create.Char('.')
        }).Tag(TAG_FIND_FIRST)),
        create.Loop(create.Identifier().Capture(CAPTURE_ENTRY)),
        create.Char(';')
    });
}

void SequenceReorder::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
}
