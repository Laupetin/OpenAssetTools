#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace phys_collmap
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::PhysCollmap>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::PhysCollmap>& asset) override;
    };
} // namespace phys_collmap
