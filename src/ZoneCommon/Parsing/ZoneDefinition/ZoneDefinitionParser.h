#pragma once

#include "Game/IGame.h"
#include "Parsing/Impl/AbstractParser.h"
#include "Zone/Definition/ZoneDefinition.h"
#include "ZoneDefinitionLexer.h"
#include "ZoneDefinitionParserState.h"
#include "ZoneDefinitionParserValue.h"

#include <optional>

class ZoneDefinitionParser final : public AbstractParser<ZoneDefinitionParserValue, ZoneDefinitionParserState>
{
protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

public:
    explicit ZoneDefinitionParser(ZoneDefinitionLexer* lexer, std::optional<GameId> maybeGame = std::nullopt);
    std::unique_ptr<ZoneDefinition> GetParsedValue();
};
