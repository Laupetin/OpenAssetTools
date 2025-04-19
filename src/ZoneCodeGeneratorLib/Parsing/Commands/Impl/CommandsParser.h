#pragma once

#include "CommandsLexer.h"
#include "CommandsParserState.h"
#include "CommandsParserValue.h"
#include "Parsing/Impl/AbstractParser.h"
#include "Persistence/IDataRepository.h"

class CommandsParser final : public AbstractParser<CommandsParserValue, CommandsParserState>
{
public:
    CommandsParser(CommandsLexer* lexer, IDataRepository* targetRepository);

protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

private:
    IDataRepository* m_repository;
};
