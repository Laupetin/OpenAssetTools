#pragma once

#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Matcher/AbstractMatcherFactory.h"

class HeaderMatcherFactory final : public AbstractMatcherFactory<HeaderParserValue>
{
public:
    explicit HeaderMatcherFactory(const IMatcherForLabelSupplier<HeaderParserValue>* labelSupplier);

    [[nodiscard]] MatcherFactoryWrapper<HeaderParserValue> Type(HeaderParserValueType type) const;
    [[nodiscard]] MatcherFactoryWrapper<HeaderParserValue> Identifier() const;
    [[nodiscard]] MatcherFactoryWrapper<HeaderParserValue> Integer() const;
    [[nodiscard]] MatcherFactoryWrapper<HeaderParserValue> FloatingPoint() const;
    [[nodiscard]] MatcherFactoryWrapper<HeaderParserValue> Char(char c) const;
};
