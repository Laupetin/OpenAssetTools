#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace font
{
    class DumperT5 final : public AbstractAssetDumper<T5::AssetFont>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetFont::Type>& asset) override;
    };
} // namespace font
