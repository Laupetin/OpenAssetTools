#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "Gdt/IGdtQueryable.h"
#include "Utils/MemoryManager.h"

namespace IW4
{
    class GdtLoaderPhysPreset final : public AssetCreator<AssetPhysPreset>
    {
    public:
        GdtLoaderPhysPreset(MemoryManager& memory, IGdtQueryable& gdt, Zone& zone);

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override;

    private:
        MemoryManager& m_memory;
        IGdtQueryable& m_gdt;
        Zone& m_zone;
    };
} // namespace IW4
