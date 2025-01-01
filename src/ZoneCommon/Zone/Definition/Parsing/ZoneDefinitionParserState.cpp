#include "ZoneDefinitionParserState.h"

#include <algorithm>

ZoneDefinitionParserState::ZoneDefinitionParserState(std::string targetName, ISearchPath& searchPath, IParserLineStream& underlyingStream)
    : m_search_path(searchPath),
      m_underlying_stream(underlyingStream),
      m_asset_name_resolver(nullptr),
      m_definition(std::make_unique<ZoneDefinition>())

{
    m_inclusions.emplace(targetName);
    m_definition->m_name = std::move(targetName);
}

void ZoneDefinitionParserState::SetGame(const GameId game)
{
    m_definition->m_game = game;
    m_asset_name_resolver = IAssetNameResolver::GetResolverForGame(game);
}

namespace
{
    void AddCurrentObjContainerToDefinitionIfNecessary(ZoneDefinition& zoneDefinition, std::optional<ZoneDefinitionObjContainer>& maybeObjContainer)
    {
        if (!maybeObjContainer)
            return;

        maybeObjContainer->m_asset_end = zoneDefinition.m_assets.size();
        zoneDefinition.m_obj_containers.emplace_back(std::move(*maybeObjContainer));
        maybeObjContainer = std::nullopt;
    }

    ZoneDefinitionObjContainer DefineNewObjContainer(const ZoneDefinition& zoneDefinition, std::string name, const ZoneDefinitionObjContainerType type)
    {
        return ZoneDefinitionObjContainer(std::move(name), type, zoneDefinition.m_assets.size());
    }

    void SortObjContainer(ZoneDefinition& zoneDefinition)
    {
        std::ranges::sort(zoneDefinition.m_obj_containers,
                          [](const ZoneDefinitionObjContainer& obj0, const ZoneDefinitionObjContainer& obj1)
                          {
                              return obj0.m_asset_start < obj1.m_asset_start;
                          });
    }
} // namespace

void ZoneDefinitionParserState::StartIPak(std::string ipakName)
{
    AddCurrentObjContainerToDefinitionIfNecessary(*m_definition, m_current_ipak);
    m_current_ipak = DefineNewObjContainer(*m_definition, std::move(ipakName), ZoneDefinitionObjContainerType::IPAK);
}

void ZoneDefinitionParserState::StartIwd(std::string iwdName)
{
    AddCurrentObjContainerToDefinitionIfNecessary(*m_definition, m_current_iwd);
    m_current_iwd = DefineNewObjContainer(*m_definition, std::move(iwdName), ZoneDefinitionObjContainerType::IWD);
}

void ZoneDefinitionParserState::Finalize()
{
    AddCurrentObjContainerToDefinitionIfNecessary(*m_definition, m_current_ipak);
    AddCurrentObjContainerToDefinitionIfNecessary(*m_definition, m_current_iwd);

    SortObjContainer(*m_definition);
}
