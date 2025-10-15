#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace phys_preset
{
    class InfoStringDumperIW4 final : public AbstractAssetDumper<IW4::AssetPhysPreset>
    {
    public:
        explicit InfoStringDumperIW4(const AssetPool<IW4::AssetPhysPreset::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetPhysPreset::Type>& asset) override;
    };
} // namespace phys_preset
