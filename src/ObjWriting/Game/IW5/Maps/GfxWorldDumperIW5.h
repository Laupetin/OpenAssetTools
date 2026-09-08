#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace gfx_world
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AssetGfxWorld>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetGfxWorld::Type>& asset) override;
    };
} // namespace gfx_world
