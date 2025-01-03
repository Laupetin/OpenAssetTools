#pragma once

#include "Game/IGame.h"
#include "Parsing/Impl/AbstractParser.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/Definition/ZoneDefinition.h"
#include "ZoneDefinitionLexer.h"
#include "ZoneDefinitionParserState.h"
#include "ZoneDefinitionParserValue.h"

#include <optional>

class ZoneDefinitionParser final : public AbstractParser<ZoneDefinitionParserValue, ZoneDefinitionParserState>
{
public:
    ZoneDefinitionParser(ZoneDefinitionLexer* lexer,
                         std::string targetName,
                         ISearchPath& searchPath,
                         IParserLineStream& underlyingStream,
                         std::optional<GameId> maybeGame = std::nullopt);
    std::unique_ptr<ZoneDefinition> GetParsedValue();

protected:
    const std::vector<sequence_t*>& GetTestsForState() override;
};
