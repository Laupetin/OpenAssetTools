#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6
{
    class AssetDumperWeaponAttachmentUnique final : public AbstractAssetDumper<WeaponAttachmentUnique>
    {
        static constexpr const char* FILE_TYPE_STR = "ATTACHMENTUNIQUEFILE";
        static constexpr const char* GDF_NAME = "attachmentunique.gdf";

        static void CopyToFullDef(const WeaponAttachmentUnique* attachment, WeaponAttachmentUniqueFull* fullDef);
        static InfoString CreateInfoString(XAssetInfo<WeaponAttachmentUnique>* asset);

    protected:
        bool ShouldDump(XAssetInfo<WeaponAttachmentUnique>* asset) override;
        bool CanDumpAsRaw() override;
        bool CanDumpAsGdtEntry() override;

        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<WeaponAttachmentUnique>* asset) override;
        GdtEntry DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<WeaponAttachmentUnique>* asset) override;
        void DumpRaw(AssetDumpingContext& context, XAssetInfo<WeaponAttachmentUnique>* asset, std::ostream& stream) override;
    };
}
