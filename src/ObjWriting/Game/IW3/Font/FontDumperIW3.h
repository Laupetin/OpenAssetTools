#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace font
{
    class FontDumperIW3 final : public AbstractAssetDumper<IW3::AssetFont>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetFont::Type>& asset) override;
    };
} // namespace font
