#include "ImageIPakPostProcessor.h"

#include "IPak/IPakCreator.h"

#include <format>

AbstractImageIPakPostProcessor::AbstractImageIPakPostProcessor(const ZoneDefinitionContext& zoneDefinition,
                                                               ISearchPath& searchPath,
                                                               const std::filesystem::path& outDir)
    : m_zone_definition(zoneDefinition),
      m_search_path(searchPath),
      m_out_dir(outDir),
      m_initialized(false),
      m_obj_container_index(0u),
      m_current_ipak(nullptr),
      m_current_ipak_start_index(0u),
      m_current_ipak_end_index(0u)
{
}

bool AbstractImageIPakPostProcessor::AppliesToZoneDefinition(const ZoneDefinitionContext& zoneDefinition)
{
    for (const auto& objContainer : zoneDefinition.m_zone_definition.m_obj_containers)
    {
        if (objContainer.m_type == ZoneDefinitionObjContainerType::IPAK)
            return true;
    }

    return false;
}

void AbstractImageIPakPostProcessor::FindNextObjContainer(AssetCreationContext& context)
{
    const auto objContainerCount = m_zone_definition.m_zone_definition.m_obj_containers.size();
    while (m_obj_container_index < objContainerCount)
    {
        const auto& objContainer = m_zone_definition.m_zone_definition.m_obj_containers[m_obj_container_index++];

        if (objContainer.m_type != ZoneDefinitionObjContainerType::IPAK)
            continue;

        auto& ipakCreator = context.GetZoneAssetCreationState<IPakCreator>();
        m_current_ipak = ipakCreator.GetOrAddIPak(objContainer.m_name);
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

    // Initialize on first image occurance
    if (!m_initialized)
    {
        FindNextObjContainer(context);
        m_initialized = true;
    }

    while (m_current_ipak && m_zone_definition.m_asset_index_in_definition >= m_current_ipak_end_index)
        FindNextObjContainer(context);

    if (m_current_ipak && m_zone_definition.m_asset_index_in_definition <= m_current_ipak_start_index)
        m_current_ipak->AddImage(assetInfo.m_name);
}

void AbstractImageIPakPostProcessor::FinalizeZone(AssetCreationContext& context)
{
    context.GetZoneAssetCreationState<IPakCreator>().Finalize(m_search_path, m_out_dir);
}
