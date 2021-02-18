#include "SequenceReusable.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceReusable::SequenceReusable()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("reusable"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Char(';')
    });
}

void SequenceReusable::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
}
