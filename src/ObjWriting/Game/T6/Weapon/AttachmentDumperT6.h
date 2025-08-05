#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace attachment
{
    class DumperT6 final : public AbstractAssetDumper<T6::WeaponAttachment>
    {
    protected:
        bool ShouldDump(XAssetInfo<T6::WeaponAttachment>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T6::WeaponAttachment>* asset) override;
    };
} // namespace attachment
