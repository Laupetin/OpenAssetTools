#pragma once

#include "Asset/IDefaultAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "Utils/MemoryManager.h"

namespace IW3
{
    class DefaultCreatorRawFile : public DefaultAssetCreator<AssetRawFile>
    {
    public:
        explicit DefaultCreatorRawFile(MemoryManager& memory);
        AssetCreationResult CreateDefaultAsset(const std::string& assetName, AssetCreationContext& context) const override;

    private:
        MemoryManager& m_memory;
    };
} // namespace IW3
