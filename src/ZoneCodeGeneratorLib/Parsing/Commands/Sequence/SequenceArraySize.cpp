#include "SequenceArraySize.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceArraySize::SequenceArraySize()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddLabeledMatchers(CommandsCommonMatchers::Evaluation(this), CommandsCommonMatchers::LABEL_EVALUATION);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("arraysize"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Label(CommandsCommonMatchers::LABEL_EVALUATION).Capture(CAPTURE_EVALUATION),
        create.Char(';')
    });
}

void SequenceArraySize::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
}
