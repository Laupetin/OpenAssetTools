#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6::attachment_unique
{
    class Dumper final : public AbstractAssetDumper<WeaponAttachmentUnique>
    {
    protected:
        bool ShouldDump(XAssetInfo<WeaponAttachmentUnique>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponAttachmentUnique>* asset) override;
    };
} // namespace T6::attachment_unique
