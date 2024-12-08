#pragma once

#include "Zone/AssetNameResolver.h"
#include "Zone/Definition/ZoneDefinition.h"

#include <memory>
#include <optional>

class ZoneDefinitionParserState
{
public:
    explicit ZoneDefinitionParserState(std::string targetName);

    void SetGame(GameId game);

    void StartIPak(std::string ipakName);
    void StartIwd(std::string iwdName);

    void Finalize();

    const IAssetNameResolver* m_asset_name_resolver;

    std::optional<ZoneDefinitionObjContainer> m_current_ipak;
    std::optional<ZoneDefinitionObjContainer> m_current_iwd;

    std::unique_ptr<ZoneDefinition> m_definition;
};
