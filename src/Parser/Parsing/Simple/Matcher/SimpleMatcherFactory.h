#pragma once

#include <string>

#include "Parsing/Simple/SimpleParserValue.h"
#include "Parsing/Matcher/AbstractMatcherFactory.h"

class SimpleMatcherFactory : public AbstractMatcherFactory<SimpleParserValue>
{
public:
    explicit SimpleMatcherFactory(const IMatcherForLabelSupplier<SimpleParserValue>* labelSupplier);
    
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Type(SimpleParserValueType type) const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Keyword(std::string value) const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> KeywordIgnoreCase(std::string value) const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> KeywordPrefix(std::string value) const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Identifier() const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> String() const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Integer() const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> IntegerWithSign() const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> FloatingPoint() const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> FloatingPointWithSign() const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> Char(char c) const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> MultiChar(int multiCharacterSequenceId) const;
    _NODISCARD MatcherFactoryWrapper<SimpleParserValue> AnyCharBesides(std::vector<char> chars) const;
};
