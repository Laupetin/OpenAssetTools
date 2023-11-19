#include "SequenceLocalizeFileConsumeEmptyLines.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceLocalizeFileConsumeEmptyLines::SequenceLocalizeFileConsumeEmptyLines()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Type(SimpleParserValueType::NEW_LINE),
    });
}

void SequenceLocalizeFileConsumeEmptyLines::ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const {}
