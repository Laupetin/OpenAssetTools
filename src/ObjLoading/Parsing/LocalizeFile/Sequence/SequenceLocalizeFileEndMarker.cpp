#include "SequenceLocalizeFileEndMarker.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceLocalizeFileEndMarker::SequenceLocalizeFileEndMarker()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Keyword("ENDMARKER")
    });
}

void SequenceLocalizeFileEndMarker::ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    state->m_end = true;
}
