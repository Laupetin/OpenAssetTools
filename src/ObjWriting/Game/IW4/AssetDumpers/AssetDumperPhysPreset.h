#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "InfoString/InfoString.h"

namespace IW4
{
    class AssetDumperPhysPreset final : public AbstractAssetDumper<PhysPreset>
    {
        static void CopyToPhysPresetInfo(const PhysPreset* physPreset, PhysPresetInfo* physPresetInfo);
        static InfoString CreateInfoString(XAssetInfo<PhysPreset>* asset);

    protected:
        bool ShouldDump(XAssetInfo<PhysPreset>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<PhysPreset>* asset) override;
    };
}
