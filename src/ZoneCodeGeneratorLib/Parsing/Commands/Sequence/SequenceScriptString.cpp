#include "SequenceScriptString.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceScriptString::SequenceScriptString()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("scriptstring"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Char(';')
    });
}

void SequenceScriptString::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
}
