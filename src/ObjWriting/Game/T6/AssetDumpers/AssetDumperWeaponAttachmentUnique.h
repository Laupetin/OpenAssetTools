#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6
{
    class AssetDumperWeaponAttachmentUnique final : public AbstractAssetDumper<WeaponAttachmentUnique>
    {
        static void CopyToFullDef(const WeaponAttachmentUnique* attachment, WeaponAttachmentUniqueFull* fullDef);
        static InfoString CreateInfoString(XAssetInfo<WeaponAttachmentUnique>* asset);

    protected:
        bool ShouldDump(XAssetInfo<WeaponAttachmentUnique>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponAttachmentUnique>* asset) override;
    };
} // namespace T6
