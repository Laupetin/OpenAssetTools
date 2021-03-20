#pragma once

#include <string>

#include "Parsing/ZoneDefinition/ZoneDefinitionParserValue.h"
#include "Parsing/Matcher/AbstractMatcherFactory.h"

class ZoneDefinitionMatcherFactory final : public AbstractMatcherFactory<ZoneDefinitionParserValue>
{
public:
    explicit ZoneDefinitionMatcherFactory(const IMatcherForLabelSupplier<ZoneDefinitionParserValue>* labelSupplier);

    _NODISCARD MatcherFactoryWrapper<ZoneDefinitionParserValue> Type(ZoneDefinitionParserValueType type) const;
    _NODISCARD MatcherFactoryWrapper<ZoneDefinitionParserValue> Keyword(std::string keyword) const;
    _NODISCARD MatcherFactoryWrapper<ZoneDefinitionParserValue> Field() const;
    _NODISCARD MatcherFactoryWrapper<ZoneDefinitionParserValue> String() const;
    _NODISCARD MatcherFactoryWrapper<ZoneDefinitionParserValue> Char(char c) const;
};
