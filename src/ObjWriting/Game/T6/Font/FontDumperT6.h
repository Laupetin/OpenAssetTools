#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace font
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetFont>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetFont::Type>& asset) override;
    };
} // namespace font
