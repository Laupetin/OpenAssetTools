#pragma once

#include "Asset/AssetCreationContext.h"
#include "Asset/IAssetCreator.h"
#include "Game/T5/T5.h"
#include "Localize/CommonLocalizeLoader.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Zone/Zone.h"

namespace T5
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
} // namespace T5