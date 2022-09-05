#pragma once

#include "DefinesDirectiveParsingState.h"
#include "Parsing/Impl/AbstractParser.h"
#include "Parsing/Simple/SimpleParserValue.h"

class DefinesDirectiveParser final : public AbstractParser<SimpleParserValue, DefinesDirectiveParsingState>
{
public:
    DefinesDirectiveParser(ILexer<SimpleParserValue>* lexer, const std::map<std::string, DefinesStreamProxy::Define>& defines);

protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

public:
    _NODISCARD std::unique_ptr<ISimpleExpression> GetParsedExpression() const;
};
