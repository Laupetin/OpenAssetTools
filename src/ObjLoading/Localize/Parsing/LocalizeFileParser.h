#pragma once

#include "Game/GameLanguage.h"
#include "LocalizeFileParserState.h"
#include "Parsing/Impl/AbstractParser.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"

class LocalizeFileParser final : public AbstractParser<SimpleParserValue, LocalizeFileParserState>
{
protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

public:
    LocalizeFileParser(SimpleLexer* lexer, GameLanguage language, LocalizeReadingZoneState* zoneState);
    std::vector<CommonLocalizeEntry> GetParsedValues();
};
