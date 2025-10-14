#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace phys_preset
{
    class InfoStringDumperT6 final : public AbstractAssetDumper<T6::PhysPreset>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::PhysPreset>& asset) override;
    };
} // namespace phys_preset
