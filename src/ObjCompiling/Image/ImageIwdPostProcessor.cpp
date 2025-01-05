#include "ImageIwdPostProcessor.h"

#include "Iwd/IwdCreator.h"

#include <algorithm>
#include <format>
#include <ranges>

AbstractImageIwdPostProcessor::AbstractImageIwdPostProcessor(const ZoneDefinitionContext& zoneDefinition,
                                                             ISearchPath& searchPath,
                                                             ZoneAssetCreationStateContainer& zoneStates,
                                                             const std::filesystem::path& outDir)
    : m_zone_definition(zoneDefinition),
      m_search_path(searchPath),
      m_iwd_creator(zoneStates.GetZoneAssetCreationState<IwdCreator>()),
      m_out_dir(outDir),
      m_obj_container_index(0u),
      m_current_iwd(nullptr),
      m_current_iwd_start_index(0u),
      m_current_iwd_end_index(0u)
{
    FindNextObjContainer();
}

bool AbstractImageIwdPostProcessor::AppliesToZoneDefinition(const ZoneDefinitionContext& zoneDefinition)
{
    return std::ranges::any_of(zoneDefinition.m_zone_definition.m_obj_containers,
                               [](const ZoneDefinitionObjContainer& objContainer)
                               {
                                   return objContainer.m_type == ZoneDefinitionObjContainerType::IWD;
                               });
}

void AbstractImageIwdPostProcessor::FindNextObjContainer()
{
    const auto objContainerCount = m_zone_definition.m_zone_definition.m_obj_containers.size();
    while (m_obj_container_index < objContainerCount)
    {
        const auto& objContainer = m_zone_definition.m_zone_definition.m_obj_containers[m_obj_container_index++];

        if (objContainer.m_type != ZoneDefinitionObjContainerType::IWD)
            continue;

        m_current_iwd = m_iwd_creator.GetOrAddIwd(objContainer.m_name);
        m_current_iwd_start_index = objContainer.m_asset_start;
        m_current_iwd_end_index = objContainer.m_asset_end;
        return;
    }

    m_current_iwd = nullptr;
}

void AbstractImageIwdPostProcessor::PostProcessAsset(XAssetInfoGeneric& assetInfo, AssetCreationContext& context)
{
    if (assetInfo.m_name.empty() || assetInfo.m_name[0] == ',')
        return;

    while (m_current_iwd && m_zone_definition.m_asset_index_in_definition >= m_current_iwd_end_index)
        FindNextObjContainer();

    if (m_current_iwd && m_zone_definition.m_asset_index_in_definition <= m_current_iwd_start_index)
        m_current_iwd->AddFile(std::format("images/{}.iwi", assetInfo.m_name));
}

void AbstractImageIwdPostProcessor::FinalizeZone(AssetCreationContext& context)
{
    m_iwd_creator.Finalize(m_search_path, m_out_dir);
}
