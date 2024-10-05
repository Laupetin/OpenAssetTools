#pragma once

#include "Zone/AssetNameResolver.h"
#include "Zone/Definition/ZoneDefinition.h"

#include <memory>

class ZoneDefinitionParserState
{
public:
    ZoneDefinitionParserState();

    void SetGame(GameId game);

    const IAssetNameResolver* m_asset_name_resolver;
    std::unique_ptr<ZoneDefinition> m_definition;
};
