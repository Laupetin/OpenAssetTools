#include "SequenceAction.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceAction::SequenceAction()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddLabeledMatchers(
        {
            create.Char('('),
            create.Optional(create.And({
                create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_ARG_TYPE),
                create.OptionalLoop(create.And({
                    create.Char(','),
                    create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_ARG_TYPE)
                }))
            })),
            create.Char(')')
        }, LABEL_ACTION_ARGS);

    AddMatchers({
        create.Keyword("set"),
        create.Keyword("action"),
        create.Or({
            create.And({
                create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
                create.Identifier().Capture(CAPTURE_ACTION_NAME)
            }),
            create.Identifier().Capture(CAPTURE_ACTION_NAME)
        }),
        create.Label(LABEL_ACTION_ARGS),
        create.Char(';')
    });
}

void SequenceAction::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
}
