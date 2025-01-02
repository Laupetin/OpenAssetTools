#include "ImageIPakPostProcessor.h"

#include "IPak/IPakCreator.h"

#include <format>

AbstractImageIPakPostProcessor::AbstractImageIPakPostProcessor(ISearchPath& searchPath, const std::filesystem::path& outDir)
    : m_search_path(searchPath),
      m_out_dir(outDir)
{
}

void AbstractImageIPakPostProcessor::PostProcessAsset(XAssetInfoGeneric& assetInfo, AssetCreationContext& context)
{
    if (assetInfo.m_name.empty() || assetInfo.m_name[0] == ',')
        return;

    auto* ipakCreator = context.GetZoneAssetLoaderState<IPakCreator>();
    ipakCreator->AddImage(assetInfo.m_name);
}

void AbstractImageIPakPostProcessor::FinalizeZone(AssetCreationContext& context)
{
    context.GetZoneAssetLoaderState<IPakCreator>()->Finalize(m_search_path, m_out_dir);
}
