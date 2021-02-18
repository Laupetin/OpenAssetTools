#pragma once

#include "CommandsLexer.h"
#include "CommandsParserState.h"
#include "CommandsParserValue.h"
#include "Parsing/Impl/AbstractParser.h"
#include "Persistence/IDataRepository.h"

class CommandsParser final : public AbstractParser<CommandsParserValue, CommandsParserState>
{
    IDataRepository* m_repository;

protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

public:
    CommandsParser(CommandsLexer* lexer, IDataRepository* targetRepository);
};
