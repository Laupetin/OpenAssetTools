#pragma once

#include "LocalizeFileParserState.h"
#include "Game/GameLanguage.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"
#include "Parsing/Impl/AbstractParser.h"

class LocalizeFileParser final : public AbstractParser<SimpleParserValue, LocalizeFileParserState>
{
protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

public:
    LocalizeFileParser(SimpleLexer* lexer, GameLanguage language);
    std::vector<LocalizeFileEntry> GetParsedValues();
};
