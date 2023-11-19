#pragma once

#include "Parsing/Menu/MenuFileParserState.h"
#include "Parsing/Sequence/SequenceResult.h"
#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

namespace menu
{
    class MenuMatcherFactory : public SimpleMatcherFactory
    {
        static constexpr auto TAG_INT = 1420;
        static constexpr auto TAG_NUMERIC = 1421;
        static constexpr auto TAG_EXPRESSION = 1422;

        static constexpr auto CAPTURE_FIRST_TOKEN = 1420;
        static constexpr auto CAPTURE_INT = 1421;
        static constexpr auto CAPTURE_NUMERIC = 1422;

    public:
        explicit MenuMatcherFactory(const IMatcherForLabelSupplier<SimpleParserValue>* labelSupplier);

        _NODISCARD MatcherFactoryWrapper<SimpleParserValue> StringChain() const;
        _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Text() const;
        _NODISCARD MatcherFactoryWrapper<SimpleParserValue> TextNoChain() const;
        _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Numeric() const;

        _NODISCARD MatcherFactoryWrapper<SimpleParserValue> IntExpression() const;
        _NODISCARD MatcherFactoryWrapper<SimpleParserValue> NumericExpression() const;

        _NODISCARD static int TokenNumericIntValue(const SimpleParserValue& value);
        _NODISCARD static double TokenNumericFloatingPointValue(const SimpleParserValue& value);
        _NODISCARD static std::string& TokenTextValue(const SimpleParserValue& value);

        _NODISCARD static int TokenIntExpressionValue(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result);
        _NODISCARD static double TokenNumericExpressionValue(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result);
    };
} // namespace menu
