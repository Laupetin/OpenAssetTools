#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace physpreset
{
    class PhysPresetDumperIW3 final : public AbstractAssetDumper<IW3::AssetPhysPreset>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetPhysPreset::Type>& asset) override;
    };
} // namespace menu
