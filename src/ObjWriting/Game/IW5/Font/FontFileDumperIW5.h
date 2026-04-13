#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace font
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AssetFont>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetFont::Type>& asset) override;
    };
} // namespace font
