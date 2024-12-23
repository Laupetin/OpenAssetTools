#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "Localize/CommonLocalizeLoader.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace IW3
{
    class AssetLoaderLocalize final : public AssetCreator<AssetLocalize>, public CommonLocalizeLoader
    {
    public:
        AssetLoaderLocalize(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override;

    protected:
        AssetCreationResult CreateAssetFromCommonAsset(const CommonLocalizeEntry& localizeEntry, AssetCreationContext& context) override;

    private:
        MemoryManager& m_memory;
    };
} // namespace IW3
