#pragma once

#include "Parsing/IParserLineStream.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/AssetNameResolver.h"
#include "Zone/Definition/ZoneDefinition.h"

#include <memory>
#include <optional>
#include <string>
#include <unordered_set>

class ZoneDefinitionParserState
{
public:
    ZoneDefinitionParserState(std::string targetName, ISearchPath& searchPath, IParserLineStream& underlyingStream);

    void SetGame(GameId game);

    void SetMapType(ZoneDefinitionMapType mapType);

    void StartIPak(std::string ipakName);
    void StartIwd(std::string iwdName);

    void Finalize();

    ISearchPath& m_search_path;
    IParserLineStream& m_underlying_stream;
    std::unordered_set<std::string> m_inclusions;

    const IAssetNameResolver* m_asset_name_resolver;

    std::optional<ZoneDefinitionObjContainer> m_current_ipak;
    std::optional<ZoneDefinitionObjContainer> m_current_iwd;

    std::unique_ptr<ZoneDefinition> m_definition;
};
