#include "SequenceEnumMember.h"

#include "Parsing/Header/Block/HeaderBlockEnum.h"
#include "Parsing/Header/Matcher/HeaderCommonMatchers.h"
#include "Parsing/Header/Matcher/HeaderMatcherFactory.h"

namespace
{
    static constexpr auto CAPTURE_NAME = 1;
    static constexpr auto CAPTURE_VALUE = 2;
} // namespace

SequenceEnumMember::SequenceEnumMember()
{
    const HeaderMatcherFactory create(this);

    AddMatchers({
        create.Identifier().Capture(CAPTURE_NAME),
        create.Optional(create.And({
            create.Char('='),
            create.Or({
                create.Identifier().Capture(CAPTURE_VALUE),
                create.Integer().Capture(CAPTURE_VALUE),
            }),
        })),
        create.Or({
            create.Char(','),
            create.Char('}').NoConsume(),
        }),
    });
}

void SequenceEnumMember::ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const
{
    int value;
    auto* enumBlock = dynamic_cast<HeaderBlockEnum*>(state->GetBlock());

    if (result.HasNextCapture(CAPTURE_VALUE))
    {
        const auto& valueToken = result.NextCapture(CAPTURE_VALUE);
        if (valueToken.m_type == HeaderParserValueType::IDENTIFIER)
        {
            auto* enumMember = enumBlock->GetEnumMember(valueToken.IdentifierValue());

            if (enumMember == nullptr)
                enumMember = state->FindEnumMember(valueToken.IdentifierValue());

            if (enumMember == nullptr)
                throw ParsingException(valueToken.GetPos(), "Could not find enum member with specified name");

            value = enumMember->m_value;
        }
        else
        {
            value = valueToken.IntegerValue();
        }
    }
    else
    {
        value = enumBlock->GetNextEnumMemberValue();
    }

    enumBlock->AddEnumMember(std::make_unique<EnumMember>(result.NextCapture(CAPTURE_NAME).IdentifierValue(), value));
}
