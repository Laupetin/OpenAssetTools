#include "ImageIwdPostProcessor.h"

#include "Iwd/IwdCreator.h"

#include <format>

AbstractImageIwdPostProcessor::AbstractImageIwdPostProcessor(ISearchPath& searchPath, const std::filesystem::path& outDir)
    : m_search_path(searchPath),
      m_out_dir(outDir)
{
}

void AbstractImageIwdPostProcessor::PostProcessAsset(XAssetInfoGeneric& assetInfo, AssetCreationContext& context)
{
    if (assetInfo.m_name.empty() || assetInfo.m_name[0] == ',')
        return;

    auto* iwdCreator = context.GetZoneAssetLoaderState<IwdCreator>();
    iwdCreator->AddFile(std::format("images/{}.iwi", assetInfo.m_name));
}

void AbstractImageIwdPostProcessor::FinalizeZone(AssetCreationContext& context)
{
    context.GetZoneAssetLoaderState<IwdCreator>()->Finalize(m_search_path, m_out_dir);
}
