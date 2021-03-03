#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperFontIcon final : public AbstractAssetDumper<FontIcon>
    {
    protected:
        bool ShouldDump(XAssetInfo<FontIcon>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<FontIcon>* asset) override;
        void DumpAsset(Zone* zone, XAssetInfo<FontIcon>* asset, std::ostream& stream) override;
    };
}
