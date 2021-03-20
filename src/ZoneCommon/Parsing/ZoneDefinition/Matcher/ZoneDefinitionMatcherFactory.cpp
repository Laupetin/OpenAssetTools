#include "ZoneDefinitionMatcherFactory.h"

#include "ZoneDefinitionMatcherCharacter.h"
#include "ZoneDefinitionMatcherKeyword.h"
#include "ZoneDefinitionMatcherValueType.h"

ZoneDefinitionMatcherFactory::ZoneDefinitionMatcherFactory(const IMatcherForLabelSupplier<ZoneDefinitionParserValue>* labelSupplier)
    : AbstractMatcherFactory(labelSupplier)
{
}

MatcherFactoryWrapper<ZoneDefinitionParserValue> ZoneDefinitionMatcherFactory::Type(ZoneDefinitionParserValueType type) const
{
    return MatcherFactoryWrapper<ZoneDefinitionParserValue>(std::make_unique<ZoneDefinitionMatcherValueType>(type));
}

MatcherFactoryWrapper<ZoneDefinitionParserValue> ZoneDefinitionMatcherFactory::Keyword(std::string keyword) const
{
    return MatcherFactoryWrapper<ZoneDefinitionParserValue>(std::make_unique<ZoneDefinitionMatcherKeyword>(std::move(keyword)));
}

MatcherFactoryWrapper<ZoneDefinitionParserValue> ZoneDefinitionMatcherFactory::Field() const
{
    return MatcherFactoryWrapper<ZoneDefinitionParserValue>(std::make_unique<ZoneDefinitionMatcherValueType>(ZoneDefinitionParserValueType::FIELD));
}

MatcherFactoryWrapper<ZoneDefinitionParserValue> ZoneDefinitionMatcherFactory::String() const
{
    return MatcherFactoryWrapper<ZoneDefinitionParserValue>(std::make_unique<ZoneDefinitionMatcherValueType>(ZoneDefinitionParserValueType::STRING));
}

MatcherFactoryWrapper<ZoneDefinitionParserValue> ZoneDefinitionMatcherFactory::Char(char c) const
{
    return MatcherFactoryWrapper<ZoneDefinitionParserValue>(std::make_unique<ZoneDefinitionMatcherCharacter>(c));
}
