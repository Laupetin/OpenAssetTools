#include "ZoneDefinitionParserState.h"

ZoneDefinitionParserState::ZoneDefinitionParserState(std::string targetName)
    : m_asset_name_resolver(nullptr),
      m_definition(std::make_unique<ZoneDefinition>())
{
    m_definition->m_name = std::move(targetName);
}

void ZoneDefinitionParserState::SetGame(const GameId game)
{
    m_definition->m_game = game;
    m_asset_name_resolver = IAssetNameResolver::GetResolverForGame(game);
}
