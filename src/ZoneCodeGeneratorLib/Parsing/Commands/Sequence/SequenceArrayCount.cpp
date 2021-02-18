#include "SequenceArrayCount.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceArrayCount::SequenceArrayCount()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddLabeledMatchers(CommandsCommonMatchers::Evaluation(this), CommandsCommonMatchers::LABEL_EVALUATION);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("arraycount"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Label(CommandsCommonMatchers::LABEL_EVALUATION).Capture(CAPTURE_EVALUATION),
        create.Char(';')
    });
}

void SequenceArrayCount::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
}
