#pragma once

#include "Parsing/Menu/MenuFileParserState.h"
#include "Parsing/Simple/Expression/SimpleExpressionMatchers.h"

#include <memory>
#include <unordered_map>

namespace menu
{
    class MenuExpressionMatchers final : public SimpleExpressionMatchers
    {
    public:
        MenuExpressionMatchers();
        explicit MenuExpressionMatchers(const MenuFileParserState* state);

    protected:
        std::unique_ptr<matcher_t> ParseOperandExtension(const supplier_t* labelSupplier) const override;
        std::unique_ptr<ISimpleExpression> ProcessOperandExtension(SequenceResult<SimpleParserValue>& result) const override;

    private:
        const MenuFileParserState* m_state;

        static const std::unordered_map<std::string, size_t>& GetBaseFunctionMapForFeatureLevel(FeatureLevel featureLevel);
    };
} // namespace menu
