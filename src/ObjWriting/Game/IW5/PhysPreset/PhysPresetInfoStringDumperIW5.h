#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace phys_preset
{
    class InfoStringDumperIW5 final : public AbstractAssetDumper<IW5::AssetPhysPreset>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetPhysPreset::Type>& asset) override;
    };
} // namespace phys_preset
