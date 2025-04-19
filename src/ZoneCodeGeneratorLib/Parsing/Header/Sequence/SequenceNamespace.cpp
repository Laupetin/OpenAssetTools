#include "SequenceNamespace.h"

#include "Parsing/Header/Block/HeaderBlockNamespace.h"
#include "Parsing/Header/Matcher/HeaderMatcherFactory.h"

namespace
{
    static constexpr int CAPTURE_NAME = 0;
}

SequenceNamespace::SequenceNamespace()
{
    const HeaderMatcherFactory create(this);

    AddMatchers({
        create.Type(HeaderParserValueType::NAMESPACE),
        create.Type(HeaderParserValueType::IDENTIFIER).Capture(CAPTURE_NAME),
        create.Char('{'),
    });
}

void SequenceNamespace::ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const
{
    assert(result.HasNextCapture(CAPTURE_NAME));
    state->PushBlock(std::make_unique<HeaderBlockNamespace>(result.NextCapture(CAPTURE_NAME).IdentifierValue()));
}
