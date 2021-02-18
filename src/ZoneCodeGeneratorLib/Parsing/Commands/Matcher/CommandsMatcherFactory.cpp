#include "CommandsMatcherFactory.h"

#include "CommandsMatcherCharacter.h"
#include "CommandsMatcherKeyword.h"
#include "CommandsMatcherValueType.h"

CommandsMatcherFactory::CommandsMatcherFactory(const IMatcherForLabelSupplier<CommandsParserValue>* labelSupplier)
    : AbstractMatcherFactory(labelSupplier)
{
}

MatcherFactoryWrapper<CommandsParserValue> CommandsMatcherFactory::Type(CommandsParserValueType type) const
{
    return MatcherFactoryWrapper<CommandsParserValue>(std::make_unique<CommandsMatcherValueType>(type));
}

MatcherFactoryWrapper<CommandsParserValue> CommandsMatcherFactory::Keyword(std::string value) const
{
    return MatcherFactoryWrapper<CommandsParserValue>(std::make_unique<CommandsMatcherKeyword>(std::move(value)));
}

MatcherFactoryWrapper<CommandsParserValue> CommandsMatcherFactory::Identifier() const
{
    return MatcherFactoryWrapper<CommandsParserValue>(std::make_unique<CommandsMatcherValueType>(CommandsParserValueType::IDENTIFIER));
}

MatcherFactoryWrapper<CommandsParserValue> CommandsMatcherFactory::Integer() const
{
    return MatcherFactoryWrapper<CommandsParserValue>(std::make_unique<CommandsMatcherValueType>(CommandsParserValueType::INTEGER));
}

MatcherFactoryWrapper<CommandsParserValue> CommandsMatcherFactory::FloatingPoint() const
{
    return MatcherFactoryWrapper<CommandsParserValue>(std::make_unique<CommandsMatcherValueType>(CommandsParserValueType::FLOATING_POINT));
}

MatcherFactoryWrapper<CommandsParserValue> CommandsMatcherFactory::Char(char c) const
{
    return MatcherFactoryWrapper<CommandsParserValue>(std::make_unique<CommandsMatcherCharacter>(c));
}
