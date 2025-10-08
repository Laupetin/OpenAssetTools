#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace attachment_unique
{
    class DumperT6 final : public AbstractAssetDumper<T6::WeaponAttachmentUnique>
    {
    protected:
        bool ShouldDump(XAssetInfo<T6::WeaponAttachmentUnique>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T6::WeaponAttachmentUnique>* asset) override;
    };
} // namespace attachment_unique
