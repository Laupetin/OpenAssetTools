#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace IW5::attachment
{
    class JsonDumper final : public AbstractAssetDumper<WeaponAttachment>
    {
    protected:
        bool ShouldDump(XAssetInfo<WeaponAttachment>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponAttachment>* asset) override;
    };
} // namespace IW5::attachment
