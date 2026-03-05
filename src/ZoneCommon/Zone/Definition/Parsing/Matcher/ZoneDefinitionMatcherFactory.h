#pragma once

#include "Parsing/Matcher/AbstractMatcherFactory.h"
#include "Zone/Definition/Parsing/ZoneDefinitionParserValue.h"

#include <string>

class ZoneDefinitionMatcherFactory final : public AbstractMatcherFactory<ZoneDefinitionParserValue>
{
public:
    explicit ZoneDefinitionMatcherFactory(const IMatcherForLabelSupplier<ZoneDefinitionParserValue>* labelSupplier);

    [[nodiscard]] MatcherFactoryWrapper<ZoneDefinitionParserValue> Type(ZoneDefinitionParserValueType type) const;
    [[nodiscard]] MatcherFactoryWrapper<ZoneDefinitionParserValue> Keyword(std::string keyword) const;
    [[nodiscard]] MatcherFactoryWrapper<ZoneDefinitionParserValue> Field() const;
    [[nodiscard]] MatcherFactoryWrapper<ZoneDefinitionParserValue> String() const;
    [[nodiscard]] MatcherFactoryWrapper<ZoneDefinitionParserValue> Char(char c) const;
};
