#pragma once

#include "Parsing/Matcher/AbstractMatcherFactory.h"
#include "Parsing/Simple/SimpleParserValue.h"

#include <string>

class SimpleMatcherFactory : public AbstractMatcherFactory<SimpleParserValue>
{
public:
    explicit SimpleMatcherFactory(const IMatcherForLabelSupplier<SimpleParserValue>* labelSupplier);

    [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> Type(SimpleParserValueType type) const;
    [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> Keyword(std::string value) const;
    [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> KeywordIgnoreCase(std::string value) const;
    [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> KeywordPrefix(std::string value) const;
    [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> Identifier() const;
    [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> String() const;
    [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> Integer() const;
    [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> IntegerWithSign() const;
    [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> FloatingPoint() const;
    [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> FloatingPointWithSign() const;
    [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> Char(char c) const;
    [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> MultiChar(int multiCharacterSequenceId) const;
    [[nodiscard]] MatcherFactoryWrapper<SimpleParserValue> AnyCharBesides(std::vector<char> chars) const;
};
