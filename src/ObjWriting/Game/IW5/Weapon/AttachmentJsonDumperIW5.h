#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace attachment
{
    class JsonDumperIW5 final : public AbstractAssetDumper<IW5::WeaponAttachment>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW5::WeaponAttachment>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW5::WeaponAttachment>* asset) override;
    };
} // namespace attachment
