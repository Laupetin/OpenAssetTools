#include "SequenceUse.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceUse::SequenceUse()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("use"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Char(';')
    });
}

void SequenceUse::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
}
