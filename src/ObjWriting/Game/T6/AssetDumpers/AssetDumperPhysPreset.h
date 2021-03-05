#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperPhysPreset final : public AbstractFileDumper<PhysPreset>
    {
        static cspField_t physpreset_fields[];

        static void CopyToPhysPresetInfo(const PhysPreset* physPreset, PhysPresetInfo* physPresetInfo);

    protected:
        bool ShouldDump(XAssetInfo<PhysPreset>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<PhysPreset>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<PhysPreset>* asset, std::ostream& stream) override;
    };
}
