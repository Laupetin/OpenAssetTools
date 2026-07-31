#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T4/T4.h"

namespace phys_preset
{
    class InfoStringDumperT4 final : public AbstractAssetDumper<T4::AssetPhysPreset>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T4::AssetPhysPreset::Type>& asset) override;
    };
} // namespace phys_preset
