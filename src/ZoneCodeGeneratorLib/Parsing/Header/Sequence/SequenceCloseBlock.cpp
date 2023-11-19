#include "SequenceCloseBlock.h"

#include "Parsing/Header/Block/IHeaderBlockNameHolder.h"
#include "Parsing/Header/Matcher/HeaderCommonMatchers.h"
#include "Parsing/Header/Matcher/HeaderMatcherFactory.h"

SequenceCloseBlock::SequenceCloseBlock(const bool semicolonRequired)
    : m_semicolon_required(semicolonRequired)
{
    const HeaderMatcherFactory create(this);

    AddMatchers({
        create.Char('}').Capture(CAPTURE_CLOSING_PARENTHESIS),
        create.Optional(create.And({
            create.Optional(create.Identifier().Capture(CAPTURE_NAME)),
            create.Char(';').Tag(TAG_SEMICOLON),
        })),
    });
}

void SequenceCloseBlock::ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const
{
    if (result.NextTag() == TAG_SEMICOLON)
    {
        if (!m_semicolon_required)
            throw ParsingException(result.NextCapture(CAPTURE_CLOSING_PARENTHESIS).GetPos(), "Block should not be closed with semicolon");
    }
    else
    {
        if (m_semicolon_required)
            throw ParsingException(result.NextCapture(CAPTURE_CLOSING_PARENTHESIS).GetPos(), "Block must be closed with semicolon");
    }

    if (result.HasNextCapture(CAPTURE_NAME))
    {
        auto* variableDefiningBlock = dynamic_cast<IHeaderBlockNameHolder*>(state->GetBlock());
        const auto& name = result.NextCapture(CAPTURE_NAME);

        if (variableDefiningBlock == nullptr)
            throw ParsingException(name.GetPos(), "Block does not support holding names.");

        variableDefiningBlock->SetBlockName(name.GetPos(), name.IdentifierValue());
    }

    state->PopBlock();
}
