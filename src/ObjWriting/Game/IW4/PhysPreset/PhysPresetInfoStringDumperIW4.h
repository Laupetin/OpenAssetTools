#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "InfoString/InfoString.h"

namespace IW4::phys_preset
{
    class InfoStringDumper final : public AbstractAssetDumper<PhysPreset>
    {
    protected:
        bool ShouldDump(XAssetInfo<PhysPreset>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<PhysPreset>* asset) override;
    };
} // namespace IW4::phys_preset
