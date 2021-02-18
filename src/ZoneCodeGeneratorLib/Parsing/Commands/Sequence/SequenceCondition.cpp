#include "SequenceCondition.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceCondition::SequenceCondition()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddLabeledMatchers(CommandsCommonMatchers::Evaluation(this), CommandsCommonMatchers::LABEL_EVALUATION);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("condition"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Or({
            create.Keyword("always").Tag(TAG_ALWAYS),
            create.Keyword("never").Tag(TAG_NEVER),
            create.Label(CommandsCommonMatchers::LABEL_EVALUATION).Tag(TAG_EVALUATION).Capture(CAPTURE_EVALUATION)
        }),
        create.Char(';')
    });
}

void SequenceCondition::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
}
