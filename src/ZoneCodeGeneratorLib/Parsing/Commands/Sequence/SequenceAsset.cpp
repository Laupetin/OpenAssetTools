#include "SequenceAsset.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceAsset::SequenceAsset()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("asset"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Identifier().Capture(CAPTURE_ENUM_ENTRY),
        create.Char(';')
    });
}

void SequenceAsset::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
}
