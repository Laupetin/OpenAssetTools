#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace clip_map
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AssetClipMap>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetClipMap::Type>& asset) override;
    };
} // namespace clip_map
