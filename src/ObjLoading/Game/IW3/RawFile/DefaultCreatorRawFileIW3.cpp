#include "DefaultCreatorRawFileIW3.h"

using namespace IW3;

DefaultCreatorRawFile::DefaultCreatorRawFile(MemoryManager& memory)
    : m_memory(memory)
{
}

AssetCreationResult DefaultCreatorRawFile::CreateDefaultAsset(const std::string& assetName, AssetCreationContext& context) const
{
    auto* asset = m_memory.Alloc<RawFile>();
    asset->name = m_memory.Dup(assetName.c_str());

    return AssetCreationResult::Success(context.AddAsset<AssetRawFile>(assetName, asset));
}
