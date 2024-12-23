#include "DefaultCreatorImageIW3.h"

using namespace IW3;

DefaultCreatorImage::DefaultCreatorImage(MemoryManager& memory)
    : m_memory(memory)
{
}

AssetCreationResult DefaultCreatorImage::CreateDefaultAsset(const std::string& assetName, AssetCreationContext& context) const
{
    auto* asset = m_memory.Alloc<GfxImage>();
    asset->name = m_memory.Dup(assetName.c_str());

    return AssetCreationResult::Success(context.AddAsset<AssetImage>(assetName, asset));
}
