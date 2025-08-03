#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6::attachment
{
    class Dumper final : public AbstractAssetDumper<WeaponAttachment>
    {
    protected:
        bool ShouldDump(XAssetInfo<WeaponAttachment>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponAttachment>* asset) override;
    };
} // namespace T6::attachment
