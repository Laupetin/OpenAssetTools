#include "SequenceLocalizeFileReference.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceLocalizeFileReference::SequenceLocalizeFileReference()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Keyword("REFERENCE"),
        create.Identifier().Capture(CAPTURE_REFERENCE_NAME),
        create.Type(SimpleParserValueType::NEW_LINE)
    });
}

void SequenceLocalizeFileReference::ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    state->m_current_reference = result.NextCapture(CAPTURE_REFERENCE_NAME).IdentifierValue();
    state->m_current_reference_languages.clear();
}
