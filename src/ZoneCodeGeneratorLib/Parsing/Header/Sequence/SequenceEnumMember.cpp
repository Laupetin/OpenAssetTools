#include "SequenceEnumMember.h"

#include "Parsing/Header/Matcher/HeaderMatcherFactory.h"
#include "Parsing/Header/Matcher/HeaderCommonMatchers.h"

SequenceEnumMember::SequenceEnumMember()
{
    const HeaderMatcherFactory create(this);

    AddMatchers({
        create.Identifier().Capture(CAPTURE_NAME),
        create.Optional(create.And({
            create.Char('='),
            create.Or({
                create.Identifier().Capture(CAPTURE_VALUE),
                create.Integer().Capture(CAPTURE_VALUE)
            })
        })),
        create.Or({
            create.Char(','),
            create.Char('}').NoConsume()
        })
    });
}

void SequenceEnumMember::ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const
{
}
