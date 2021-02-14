#include "SequenceEnum.h"

#include "Parsing/Header/Block/HeaderBlockEnum.h"
#include "Parsing/Header/Matcher/HeaderMatcherFactory.h"
#include "Parsing/Header/Matcher/HeaderCommonMatchers.h"

SequenceEnum::SequenceEnum()
{
    const HeaderMatcherFactory create(this);

    AddLabeledMatchers(HeaderCommonMatchers::Typename(this), HeaderCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Optional(create.Type(HeaderParserValueType::TYPEDEF).Tag(TAG_TYPEDEF)),
        create.Type(HeaderParserValueType::ENUM),
        create.Optional(create.Identifier().Capture(CAPTURE_NAME)),
        create.Optional(create.And({
            create.Char(':'),
            create.Label(HeaderCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_PARENT_TYPE)
        })),
        create.Char('{')
    });
}

void SequenceEnum::ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const
{
    state->PushBlock(std::make_unique<HeaderBlockEnum>());
}
