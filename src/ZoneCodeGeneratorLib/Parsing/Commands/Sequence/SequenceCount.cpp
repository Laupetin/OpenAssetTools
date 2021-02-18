#include "SequenceCount.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceCount::SequenceCount()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddLabeledMatchers(CommandsCommonMatchers::Evaluation(this), CommandsCommonMatchers::LABEL_EVALUATION);
    AddLabeledMatchers(CommandsCommonMatchers::ArrayDef(this), CommandsCommonMatchers::LABEL_ARRAY_DEF);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("count"),
        create.OptionalLoop(create.Char('*').Tag(TAG_POINTER_RESOLVE)),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.OptionalLoop(create.Label(CommandsCommonMatchers::LABEL_ARRAY_DEF).Capture(CAPTURE_ARRAY_INDEX)),
        create.Label(CommandsCommonMatchers::LABEL_EVALUATION).Capture(CAPTURE_EVALUATION),
        create.Char(';')
    });
}

void SequenceCount::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
}
