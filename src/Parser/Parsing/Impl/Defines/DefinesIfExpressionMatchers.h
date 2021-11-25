#pragma once

#include <memory>

#include "DefinesIfDirectiveParsingState.h"
#include "Parsing/Simple/Expression/SimpleExpressionMatchers.h"

class DefinesIfExpressionMatchers final : public SimpleExpressionMatchers
{
    static constexpr auto CAPTURE_DEFINE_NAME = CAPTURE_OFFSET_EXPRESSION_EXT + 1;

    const DefinesIfDirectiveParsingState* m_state;

protected:
    std::unique_ptr<matcher_t> ParseOperandExtension(const supplier_t* labelSupplier) const override;
    std::unique_ptr<ISimpleExpression> ProcessOperandExtension(SequenceResult<SimpleParserValue>& result) const override;

public:
    DefinesIfExpressionMatchers();
    explicit DefinesIfExpressionMatchers(const DefinesIfDirectiveParsingState* state);
};
