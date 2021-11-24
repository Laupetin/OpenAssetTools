#include "MenuExpressionMatchers.h"

#include "MenuMatcherFactory.h"
#include "Parsing/Menu/Domain/Expression/CommonExpressionFunctionCall.h"

using namespace menu;

static constexpr int TAG_EXPRESSION_FUNCTION_CALL = SimpleExpressionMatchers::TAG_OFFSET_EXPRESSION_EXT + 1;
static constexpr int TAG_EXPRESSION_FUNCTION_CALL_END = SimpleExpressionMatchers::TAG_OFFSET_EXPRESSION_EXT + 2;

static constexpr int CAPTURE_FUNCTION_NAME = SimpleExpressionMatchers::CAPTURE_OFFSET_EXPRESSION_EXT + 1;


MenuExpressionMatchers::MenuExpressionMatchers()
    : SimpleExpressionMatchers(true, true, true, true)
{
}

std::unique_ptr<SimpleExpressionMatchers::matcher_t> MenuExpressionMatchers::ParseOperandExtension(const supplier_t* labelSupplier) const
{
    const MenuMatcherFactory create(labelSupplier);

    return create.Or({
        create.And({
            create.Identifier().Capture(CAPTURE_FUNCTION_NAME),
            create.Char('('),
            create.Optional(create.And({
                create.Label(LABEL_EXPRESSION),
                create.OptionalLoop(create.And({
                    create.Char(','),
                    create.Label(LABEL_EXPRESSION)
                })),
            })),
            create.Char(')').Tag(TAG_EXPRESSION_FUNCTION_CALL_END)
        }).Tag(TAG_EXPRESSION_FUNCTION_CALL)
    });
}

std::unique_ptr<ISimpleExpression> MenuExpressionMatchers::ProcessOperandExtension(SequenceResult<SimpleParserValue>& result) const
{
    auto functionCall = std::make_unique<CommonExpressionFunctionCall>(result.NextCapture(CAPTURE_FUNCTION_NAME).IdentifierValue());

    while (result.PeekAndRemoveIfTag(TAG_EXPRESSION_FUNCTION_CALL_END) != TAG_EXPRESSION_FUNCTION_CALL_END)
    {
        functionCall->m_args.emplace_back(ProcessExpression(result));
    }

    return std::move(functionCall);
}