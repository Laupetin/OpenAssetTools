#pragma once

#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Matcher/AbstractMatcherFactory.h"

class HeaderMatcherFactory final : public AbstractMatcherFactory<HeaderParserValue>
{
public:
    explicit HeaderMatcherFactory(const IMatcherForLabelSupplier<HeaderParserValue>* labelSupplier);

    _NODISCARD MatcherFactoryWrapper<HeaderParserValue> Type(HeaderParserValueType type) const;
    _NODISCARD MatcherFactoryWrapper<HeaderParserValue> Char(char c) const;
};
