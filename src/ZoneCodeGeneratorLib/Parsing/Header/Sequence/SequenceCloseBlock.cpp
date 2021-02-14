#include "SequenceCloseBlock.h"

#include "Parsing/Header/Matcher/HeaderMatcherFactory.h"
#include "Parsing/Header/Matcher/HeaderCommonMatchers.h"

SequenceCloseBlock::SequenceCloseBlock()
{
    const HeaderMatcherFactory create(this);

    AddMatchers({
        create.Char('}'),
        create.Optional(create.And({
            create.Optional(create.Identifier().Capture(CAPTURE_NAME)),
            create.Char(';').Tag(TAG_SEMICOLON)
        }))
    });
}

void SequenceCloseBlock::ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const
{
    // TODO: Set variable name to last block
    state->PopBlock();
}
