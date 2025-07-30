#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6::phys_preset
{
    class InfoStringDumper final : public AbstractAssetDumper<PhysPreset>
    {
    protected:
        bool ShouldDump(XAssetInfo<PhysPreset>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<PhysPreset>* asset) override;
    };
} // namespace T6::phys_preset
