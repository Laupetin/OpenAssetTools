#include "SequenceLocalizeFileReference.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceLocalizeFileReference::SequenceLocalizeFileReference()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Keyword("REFERENCE"),
        create.Or({create.Identifier(), create.String(),}).Capture(CAPTURE_REFERENCE_NAME),
        create.Type(SimpleParserValueType::NEW_LINE),
    });
}

void SequenceLocalizeFileReference::ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    const auto& nameToken = result.NextCapture(CAPTURE_REFERENCE_NAME);
    state->m_current_reference = nameToken.m_type == SimpleParserValueType::IDENTIFIER ? nameToken.IdentifierValue() : nameToken.StringValue();
    state->m_current_reference_languages.clear();
}
