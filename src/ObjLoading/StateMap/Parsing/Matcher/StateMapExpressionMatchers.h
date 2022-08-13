#pragma once

#include <memory>

#include "Parsing/Simple/Expression/SimpleExpressionMatchers.h"

namespace state_map
{
    class StateMapExpressionMatchers final : public SimpleExpressionMatchers
    {
    public:
        StateMapExpressionMatchers();

    protected:
        std::unique_ptr<matcher_t> ParseOperandExtension(const supplier_t* labelSupplier) const override;
        std::unique_ptr<ISimpleExpression> ProcessOperandExtension(SequenceResult<SimpleParserValue>& result) const override;
    };
}
