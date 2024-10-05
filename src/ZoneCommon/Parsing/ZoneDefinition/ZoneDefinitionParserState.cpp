#include "ZoneDefinitionParserState.h"

ZoneDefinitionParserState::ZoneDefinitionParserState()
    : m_asset_name_resolver(nullptr),
      m_definition(std::make_unique<ZoneDefinition>())
{
}

void ZoneDefinitionParserState::SetGame(const GameId game)
{
    m_definition->m_game = game;
    m_asset_name_resolver = IAssetNameResolver::GetResolverForGame(game);
}
