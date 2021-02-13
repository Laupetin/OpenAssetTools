#pragma once

#include "HeaderLexer.h"
#include "HeaderParserState.h"
#include "Parsing/Impl/AbstractParser.h"
#include "Persistence/IDataRepository.h"

class HeaderParser final : public AbstractParser<HeaderParserValue, HeaderParserState>
{
    IDataRepository* m_repository;

    std::vector<std::unique_ptr<sequence_t>> m_tests;
    std::vector<sequence_t*> m_normal_tests;

protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

public:
    HeaderParser(HeaderLexer* lexer, IDataRepository* targetRepository);
};
