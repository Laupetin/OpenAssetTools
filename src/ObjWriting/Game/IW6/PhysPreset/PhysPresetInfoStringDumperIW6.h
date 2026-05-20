#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW6/IW6.h"

namespace phys_preset
{
    class InfoStringDumperIW6 final : public AbstractAssetDumper<IW6::AssetPhysPreset>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW6::AssetPhysPreset::Type>& asset) override;
    };
} // namespace phys_preset
