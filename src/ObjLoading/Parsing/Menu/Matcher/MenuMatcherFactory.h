#pragma once

#include "Parsing/Menu/MenuFileParserState.h"
#include "Parsing/Sequence/SequenceResult.h"
#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

namespace menu
{
    class MenuMatcherFactory : public SimpleMatcherFactory
    {
        static constexpr auto TAG_STRING_CHAIN = 1420;
        static constexpr auto TAG_IDENTIFIER = 1421;
        static constexpr auto TAG_INT = 1422;
        static constexpr auto TAG_NUMERIC = 1423;
        static constexpr auto TAG_EXPRESSION = 1424;

        static constexpr auto CAPTURE_FIRST_TOKEN = 1420;
        static constexpr auto CAPTURE_STRING_CHAIN = 1421;
        static constexpr auto CAPTURE_IDENTIFIER = 1422;
        static constexpr auto CAPTURE_INT = 1423;
        static constexpr auto CAPTURE_NUMERIC = 1424;

    public:
        explicit MenuMatcherFactory(const IMatcherForLabelSupplier<SimpleParserValue>* labelSupplier);

        [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> StringChain() const;
        [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> Text() const;
        [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> TextNoChain() const;
        [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> Numeric() const;

        [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> TextExpression() const;
        [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> IntExpression() const;
        [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> NumericExpression() const;

        [[nodiscard]] static int TokenNumericIntValue(const SimpleParserValue& value);
        [[nodiscard]] static double TokenNumericFloatingPointValue(const SimpleParserValue& value);
        [[nodiscard]] static std::string& TokenTextValue(const SimpleParserValue& value);

        [[nodiscard]] static std::string TokenTextExpressionValue(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result);
        [[nodiscard]] static int TokenIntExpressionValue(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result);
        [[nodiscard]] static double TokenNumericExpressionValue(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result);
    };
} // namespace menu
