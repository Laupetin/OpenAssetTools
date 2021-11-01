#pragma once

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

namespace menu
{
    class MenuMatcherFactory : public SimpleMatcherFactory
    {
    public:
        explicit MenuMatcherFactory(const IMatcherForLabelSupplier<SimpleParserValue>* labelSupplier);

        _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Text() const;
        _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Numeric() const;

        _NODISCARD static int TokenNumericIntValue(const SimpleParserValue& value);
        _NODISCARD static double TokenNumericFloatingPointValue(const SimpleParserValue& value);
        _NODISCARD static std::string& TokenTextValue(const SimpleParserValue& value);
    };
}
