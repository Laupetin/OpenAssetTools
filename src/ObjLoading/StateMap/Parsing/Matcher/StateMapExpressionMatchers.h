#pragma once

#include <memory>

#include "Parsing/Simple/Expression/SimpleExpressionMatchers.h"
#include "StateMap/Parsing/StateMapParserState.h"

namespace state_map
{
    class StateMapExpressionMatchers final : public SimpleExpressionMatchers
    {
    public:
        StateMapExpressionMatchers();
        explicit StateMapExpressionMatchers(StateMapParserState* state);

    protected:
        std::unique_ptr<matcher_t> ParseOperandExtension(const supplier_t* labelSupplier) const override;
        std::unique_ptr<ISimpleExpression> ProcessOperandExtension(SequenceResult<SimpleParserValue>& result) const override;

    private:
        StateMapParserState* m_state;
    };
}
