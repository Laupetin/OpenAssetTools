#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6
{
    class AssetDumperPhysPreset final : public AbstractAssetDumper<PhysPreset>
    {
        static void CopyToPhysPresetInfo(const PhysPreset* physPreset, PhysPresetInfo* physPresetInfo);
        static InfoString CreateInfoString(XAssetInfo<PhysPreset>* asset);

    protected:
        bool ShouldDump(XAssetInfo<PhysPreset>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<PhysPreset>* asset) override;
    };
} // namespace T6
