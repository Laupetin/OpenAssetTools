#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperFontIcon final : public AbstractAssetDumper<T6::FontIcon>
    {
    protected:
        bool ShouldDump(T6::FontIcon* asset) override;
        std::string GetFileNameForAsset(Zone* zone, T6::FontIcon* asset) override;
        void DumpAsset(Zone* zone, T6::FontIcon* asset, FileAPI::File* out) override;
    };
}
