#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace phys_preset
{
    class InfoStringDumperT5 final : public AbstractAssetDumper<T5::AssetPhysPreset>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetPhysPreset::Type>& asset) override;
    };
} // namespace phys_preset
