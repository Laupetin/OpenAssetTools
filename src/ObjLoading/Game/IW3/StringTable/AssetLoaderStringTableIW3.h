#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace IW3
{
    class AssetLoaderStringTable final : public AssetCreator<AssetRawFile>
    {
    public:
        AssetLoaderStringTable(MemoryManager& memory, ISearchPath& searchPath);

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override;

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace IW3
