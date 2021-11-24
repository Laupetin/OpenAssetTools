#pragma once

#include <memory>

#include "Parsing/Simple/Expression/SimpleExpressionMatchers.h"

namespace menu
{
    class MenuExpressionMatchers final : public SimpleExpressionMatchers
    {
    public:
        MenuExpressionMatchers();

    protected:
        std::unique_ptr<matcher_t> ParseOperandExtension(const supplier_t* labelSupplier) const override;
        std::unique_ptr<ISimpleExpression> ProcessOperandExtension(SequenceResult<SimpleParserValue>& result) const override;
    };
}
