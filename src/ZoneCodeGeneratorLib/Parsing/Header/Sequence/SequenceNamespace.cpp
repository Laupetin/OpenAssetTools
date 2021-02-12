#include "SequenceNamespace.h"

#include "Parsing/Header/Matcher/HeaderMatcherFactory.h"

SequenceNamespace::SequenceNamespace()
{
    const HeaderMatcherFactory create(this);

    AddMatchers({
        create.Type(HeaderParserValueType::NAMESPACE),
        create.Type(HeaderParserValueType::IDENTIFIER).Capture(CAPTURE_NAME),
        create.Char('{')
    });
}

void SequenceNamespace::ProcessMatch(HeaderParserState* state, const SequenceResult<HeaderParserValue>& result) const
{
}
