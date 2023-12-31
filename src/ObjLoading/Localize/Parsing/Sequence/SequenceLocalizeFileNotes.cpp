#include "SequenceLocalizeFileNotes.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceLocalizeFileNotes::SequenceLocalizeFileNotes()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Keyword("FILENOTES"),
        create.String(),
        create.Type(SimpleParserValueType::NEW_LINE),
    });
}

void SequenceLocalizeFileNotes::ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const {}
