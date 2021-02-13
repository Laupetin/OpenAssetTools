#pragma once

#include "HeaderLexer.h"
#include "HeaderParserState.h"
#include "Parsing/Impl/AbstractParser.h"
#include "Persistence/IDataRepository.h"

class HeaderParser final : public AbstractParser<HeaderParserValue, HeaderParserState>
{
    IDataRepository* m_repository;

protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

public:
    HeaderParser(HeaderLexer* lexer, IDataRepository* targetRepository);
};
