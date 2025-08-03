#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4::phys_collmap
{
    class Dumper final : public AbstractAssetDumper<PhysCollmap>
    {
    protected:
        bool ShouldDump(XAssetInfo<PhysCollmap>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<PhysCollmap>* asset) override;
    };
} // namespace IW4::phys_collmap
