#include "SimpleMatcherFactory.h"

#include "SimpleMatcherAnyCharacterBesides.h"
#include "SimpleMatcherCharacter.h"
#include "SimpleMatcherKeyword.h"
#include "SimpleMatcherKeywordIgnoreCase.h"
#include "SimpleMatcherKeywordPrefix.h"
#include "SimpleMatcherMultiCharacter.h"
#include "SimpleMatcherValueType.h"

SimpleMatcherFactory::SimpleMatcherFactory(const IMatcherForLabelSupplier<SimpleParserValue>* labelSupplier)
    : AbstractMatcherFactory(labelSupplier)
{
}

MatcherFactoryWrapper<SimpleParserValue> SimpleMatcherFactory::Type(SimpleParserValueType type) const
{
    return MatcherFactoryWrapper<SimpleParserValue>(std::make_unique<SimpleMatcherValueType>(type));
}

MatcherFactoryWrapper<SimpleParserValue> SimpleMatcherFactory::Keyword(std::string value) const
{
    return MatcherFactoryWrapper<SimpleParserValue>(std::make_unique<SimpleMatcherKeyword>(std::move(value)));
}

MatcherFactoryWrapper<SimpleParserValue> SimpleMatcherFactory::KeywordIgnoreCase(std::string value) const
{
    return MatcherFactoryWrapper<SimpleParserValue>(std::make_unique<SimpleMatcherKeywordIgnoreCase>(std::move(value)));
}

MatcherFactoryWrapper<SimpleParserValue> SimpleMatcherFactory::KeywordPrefix(std::string value) const
{
    return MatcherFactoryWrapper<SimpleParserValue>(std::make_unique<SimpleMatcherKeywordPrefix>(std::move(value)));
}

MatcherFactoryWrapper<SimpleParserValue> SimpleMatcherFactory::Identifier() const
{
    return MatcherFactoryWrapper<SimpleParserValue>(std::make_unique<SimpleMatcherValueType>(SimpleParserValueType::IDENTIFIER));
}

MatcherFactoryWrapper<SimpleParserValue> SimpleMatcherFactory::String() const
{
    return MatcherFactoryWrapper<SimpleParserValue>(std::make_unique<SimpleMatcherValueType>(SimpleParserValueType::STRING));
}

MatcherFactoryWrapper<SimpleParserValue> SimpleMatcherFactory::Integer() const
{
    return MatcherFactoryWrapper<SimpleParserValue>(std::make_unique<SimpleMatcherValueType>(SimpleParserValueType::INTEGER));
}

MatcherFactoryWrapper<SimpleParserValue> SimpleMatcherFactory::FloatingPoint() const
{
    return MatcherFactoryWrapper<SimpleParserValue>(std::make_unique<SimpleMatcherValueType>(SimpleParserValueType::FLOATING_POINT));
}

MatcherFactoryWrapper<SimpleParserValue> SimpleMatcherFactory::Char(char c) const
{
    return MatcherFactoryWrapper<SimpleParserValue>(std::make_unique<SimpleMatcherCharacter>(c));
}

MatcherFactoryWrapper<SimpleParserValue> SimpleMatcherFactory::MultiChar(int multiCharacterSequenceId) const
{
    return MatcherFactoryWrapper<SimpleParserValue>(std::make_unique<SimpleMatcherMultiCharacter>(multiCharacterSequenceId));
}

MatcherFactoryWrapper<SimpleParserValue> SimpleMatcherFactory::AnyCharBesides(std::vector<char> chars) const
{
    return MatcherFactoryWrapper<SimpleParserValue>(std::make_unique<SimpleMatcherAnyCharacterBesides>(std::move(chars)));
}
