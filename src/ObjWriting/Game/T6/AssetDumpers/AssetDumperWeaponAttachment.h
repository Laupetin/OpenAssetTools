#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6
{
    class AssetDumperWeaponAttachment final : public AbstractAssetDumper<WeaponAttachment>
    {
        static constexpr const char* FILE_TYPE_STR = "ATTACHMENTFILE";
        static constexpr const char* GDF_NAME = "attachment.gdf";
        static cspField_t attachment_fields[];
        
        static InfoString CreateInfoString(XAssetInfo<WeaponAttachment>* asset);

    protected:
        bool ShouldDump(XAssetInfo<WeaponAttachment>* asset) override;
        bool CanDumpAsRaw() override;
        bool CanDumpAsGdtEntry() override;

        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<WeaponAttachment>* asset) override;
        GdtEntry DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<WeaponAttachment>* asset) override;
        void DumpRaw(AssetDumpingContext& context, XAssetInfo<WeaponAttachment>* asset, std::ostream& stream) override;
    };
}
