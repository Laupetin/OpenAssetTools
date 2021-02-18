#pragma once

#include "HeaderLexer.h"
#include "HeaderParserState.h"
#include "Parsing/Impl/AbstractParser.h"
#include "Persistence/IDataRepository.h"

class HeaderParser final : public AbstractParser<HeaderParserValue, HeaderParserState>
{
protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

public:
    explicit HeaderParser(HeaderLexer* lexer);

    void SaveToRepository(IDataRepository* repository);
};
