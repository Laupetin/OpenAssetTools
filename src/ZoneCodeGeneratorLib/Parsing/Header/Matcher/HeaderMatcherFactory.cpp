#include "HeaderMatcherFactory.h"

#include "HeaderMatcherCharacter.h"
#include "HeaderMatcherValueType.h"

HeaderMatcherFactory::HeaderMatcherFactory(const IMatcherForLabelSupplier<HeaderParserValue>* labelSupplier)
    : AbstractMatcherFactory(labelSupplier)
{
}

MatcherFactoryWrapper<HeaderParserValue> HeaderMatcherFactory::Type(HeaderParserValueType type) const
{
    return MatcherFactoryWrapper<HeaderParserValue>(std::make_unique<HeaderMatcherValueType>(type));
}

MatcherFactoryWrapper<HeaderParserValue> HeaderMatcherFactory::Identifier() const
{
    return MatcherFactoryWrapper<HeaderParserValue>(std::make_unique<HeaderMatcherValueType>(HeaderParserValueType::IDENTIFIER));
}

MatcherFactoryWrapper<HeaderParserValue> HeaderMatcherFactory::Integer() const
{
    return MatcherFactoryWrapper<HeaderParserValue>(std::make_unique<HeaderMatcherValueType>(HeaderParserValueType::INTEGER));
}

MatcherFactoryWrapper<HeaderParserValue> HeaderMatcherFactory::FloatingPoint() const
{
    return MatcherFactoryWrapper<HeaderParserValue>(std::make_unique<HeaderMatcherValueType>(HeaderParserValueType::FLOATING_POINT));
}

MatcherFactoryWrapper<HeaderParserValue> HeaderMatcherFactory::Char(char c) const
{
    return MatcherFactoryWrapper<HeaderParserValue>(std::make_unique<HeaderMatcherCharacter>(c));
}
