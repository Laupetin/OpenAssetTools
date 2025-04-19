#pragma once

#include "Parsing/Commands/Impl/CommandsParserValue.h"
#include "Parsing/Matcher/AbstractMatcherFactory.h"

#include <string>

class CommandsMatcherFactory final : public AbstractMatcherFactory<CommandsParserValue>
{
public:
    explicit CommandsMatcherFactory(const IMatcherForLabelSupplier<CommandsParserValue>* labelSupplier);

    [[nodiscard]] MatcherFactoryWrapper<CommandsParserValue> Type(CommandsParserValueType type) const;
    [[nodiscard]] MatcherFactoryWrapper<CommandsParserValue> Keyword(std::string value) const;
    [[nodiscard]] MatcherFactoryWrapper<CommandsParserValue> Identifier() const;
    [[nodiscard]] MatcherFactoryWrapper<CommandsParserValue> Integer() const;
    [[nodiscard]] MatcherFactoryWrapper<CommandsParserValue> FloatingPoint() const;
    [[nodiscard]] MatcherFactoryWrapper<CommandsParserValue> Char(char c) const;
};
