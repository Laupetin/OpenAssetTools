#include "AssetLoaderLocalizeIW4.h"

using namespace IW4;

AssetLoaderLocalize::AssetLoaderLocalize(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    : CommonLocalizeLoader(searchPath, zone),
      m_memory(memory)
{
}

AssetCreationResult AssetLoaderLocalize::CreateAsset(const std::string& assetName, AssetCreationContext& context)
{
    return CreateLocalizeAsset(assetName, context);
}

AssetCreationResult AssetLoaderLocalize::CreateAssetFromCommonAsset(const CommonLocalizeEntry& localizeEntry, AssetCreationContext& context)
{
    auto* asset = m_memory.Alloc<LocalizeEntry>();
    asset->name = m_memory.Dup(localizeEntry.m_key.c_str());
    asset->value = m_memory.Dup(localizeEntry.m_value.c_str());

    return AssetCreationResult::Success(context.AddAsset<AssetLocalize>(localizeEntry.m_key, asset));
}
