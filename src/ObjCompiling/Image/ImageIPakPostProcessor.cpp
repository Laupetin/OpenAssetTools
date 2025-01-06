#include "ImageIPakPostProcessor.h"

#include "IPak/IPakCreator.h"

#include <algorithm>
#include <ranges>

AbstractImageIPakPostProcessor::AbstractImageIPakPostProcessor(const ZoneDefinitionContext& zoneDefinition,
                                                               ISearchPath& searchPath,
                                                               ZoneAssetCreationStateContainer& zoneStates,
                                                               IOutputPath& outDir)
    : m_zone_definition(zoneDefinition),
      m_search_path(searchPath),
      m_ipak_creator(zoneStates.GetZoneAssetCreationState<IPakCreator>()),
      m_out_dir(outDir),
      m_obj_container_index(0u),
      m_current_ipak(nullptr),
      m_current_ipak_start_index(0u),
      m_current_ipak_end_index(0u)
{
    FindNextObjContainer();
}

bool AbstractImageIPakPostProcessor::AppliesToZoneDefinition(const ZoneDefinitionContext& zoneDefinition)
{
    return std::ranges::any_of(zoneDefinition.m_zone_definition.m_obj_containers,
                               [](const ZoneDefinitionObjContainer& objContainer)
                               {
                                   return objContainer.m_type == ZoneDefinitionObjContainerType::IPAK;
                               });
}

void AbstractImageIPakPostProcessor::FindNextObjContainer()
{
    const auto objContainerCount = m_zone_definition.m_zone_definition.m_obj_containers.size();
    while (m_obj_container_index < objContainerCount)
    {
        const auto& objContainer = m_zone_definition.m_zone_definition.m_obj_containers[m_obj_container_index++];

        if (objContainer.m_type != ZoneDefinitionObjContainerType::IPAK)
            continue;

        m_current_ipak = m_ipak_creator.GetOrAddIPak(objContainer.m_name);
        m_current_ipak_start_index = objContainer.m_asset_start;
        m_current_ipak_end_index = objContainer.m_asset_end;
        return;
    }

    m_current_ipak = nullptr;
}

void AbstractImageIPakPostProcessor::PostProcessAsset(XAssetInfoGeneric& assetInfo, AssetCreationContext& context)
{
    if (assetInfo.m_name.empty() || assetInfo.m_name[0] == ',')
        return;

    while (m_current_ipak && m_zone_definition.m_asset_index_in_definition >= m_current_ipak_end_index)
        FindNextObjContainer();

    if (m_current_ipak && m_zone_definition.m_asset_index_in_definition <= m_current_ipak_start_index)
        m_current_ipak->AddImage(assetInfo.m_name);
}

void AbstractImageIPakPostProcessor::FinalizeZone(AssetCreationContext& context)
{
    m_ipak_creator.Finalize(m_search_path, m_out_dir);
}
