#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperPhysPreset final : public AbstractAssetDumper<PhysPreset>
    {
        static cspField_t physpreset_fields[];

        static void CopyToPhysPresetInfo(const PhysPreset* physPreset, PhysPresetInfo* physPresetInfo);

    protected:
        bool ShouldDump(XAssetInfo<PhysPreset>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<PhysPreset>* asset) override;
        void DumpAsset(Zone* zone, XAssetInfo<PhysPreset>* asset, FileAPI::File* out) override;
    };
}
