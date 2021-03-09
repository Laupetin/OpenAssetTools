#pragma once

#include <string>

#include "Parsing/Simple/SimpleParserValue.h"
#include "Parsing/Matcher/AbstractMatcherFactory.h"

class SimpleMatcherFactory final : public AbstractMatcherFactory<SimpleParserValue>
{
public:
    explicit SimpleMatcherFactory(const IMatcherForLabelSupplier<SimpleParserValue>* labelSupplier);

    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Type(SimpleParserValueType type) const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Keyword(std::string value) const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Identifier() const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Integer() const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> FloatingPoint() const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Char(char c) const;
};
