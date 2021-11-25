#pragma once

#include "DefinesIfDirectiveParsingState.h"
#include "Parsing/Impl/AbstractParser.h"
#include "Parsing/Simple/SimpleParserValue.h"

class DefinesIfDirectiveParser final : public AbstractParser<SimpleParserValue, DefinesIfDirectiveParsingState>
{
public:
    DefinesIfDirectiveParser(ILexer<SimpleParserValue>* lexer, const std::map<std::string, DefinesStreamProxy::Define>& defines);

protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

public:
    _NODISCARD std::unique_ptr<ISimpleExpression> GetParsedExpression() const;
};
