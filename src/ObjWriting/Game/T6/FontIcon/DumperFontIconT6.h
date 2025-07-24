#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperFontIcon final : public AbstractAssetDumper<FontIcon>
    {
    protected:
        bool ShouldDump(XAssetInfo<FontIcon>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<FontIcon>* asset) override;
    };
} // namespace T6
