#pragma once

#include <memory>

#include "DefinesDirectiveParsingState.h"
#include "Parsing/Simple/Expression/SimpleExpressionMatchers.h"

class DefinesExpressionMatchers final : public SimpleExpressionMatchers
{
    static constexpr auto CAPTURE_DEFINE_NAME = CAPTURE_OFFSET_EXPRESSION_EXT + 1;

    const DefinesDirectiveParsingState* m_state;

protected:
    std::unique_ptr<matcher_t> ParseOperandExtension(const supplier_t* labelSupplier) const override;
    std::unique_ptr<ISimpleExpression> ProcessOperandExtension(SequenceResult<SimpleParserValue>& result) const override;

public:
    DefinesExpressionMatchers();
    explicit DefinesExpressionMatchers(const DefinesDirectiveParsingState* state);
};
