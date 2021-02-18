#pragma once

#include "HeaderLexer.h"
#include "HeaderParserState.h"
#include "Parsing/IPackValueSupplier.h"
#include "Parsing/Impl/AbstractParser.h"
#include "Persistence/IDataRepository.h"

class HeaderParser final : public AbstractParser<HeaderParserValue, HeaderParserState>
{
protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

public:
    HeaderParser(HeaderLexer* lexer, const IPackValueSupplier* packValueSupplier);

    void SaveToRepository(IDataRepository* repository) const;
};
