#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperFontIcon final : public AbstractFileDumper<FontIcon>
    {
    protected:
        bool ShouldDump(XAssetInfo<FontIcon>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<FontIcon>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<FontIcon>* asset, std::ostream& stream) override;
    };
}
