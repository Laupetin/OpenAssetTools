#pragma once

#include "Parsing/Commands/Impl/CommandsParserValue.h"
#include "Parsing/Matcher/AbstractMatcherFactory.h"

#include <string>

class CommandsMatcherFactory final : public AbstractMatcherFactory<CommandsParserValue>
{
public:
    explicit CommandsMatcherFactory(const IMatcherForLabelSupplier<CommandsParserValue>* labelSupplier);

    _NODISCARD MatcherFactoryWrapper<CommandsParserValue> Type(CommandsParserValueType type) const;
    _NODISCARD MatcherFactoryWrapper<CommandsParserValue> Keyword(std::string value) const;
    _NODISCARD MatcherFactoryWrapper<CommandsParserValue> Identifier() const;
    _NODISCARD MatcherFactoryWrapper<CommandsParserValue> Integer() const;
    _NODISCARD MatcherFactoryWrapper<CommandsParserValue> FloatingPoint() const;
    _NODISCARD MatcherFactoryWrapper<CommandsParserValue> Char(char c) const;
};
