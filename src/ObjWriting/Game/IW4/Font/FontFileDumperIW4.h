#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace font
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetFont>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetFont::Type>& asset) override;
    };
} // namespace font
