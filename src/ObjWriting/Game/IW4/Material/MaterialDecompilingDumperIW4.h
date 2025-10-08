#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace material
{
    class DecompilingGdtDumperIW4 final : public AbstractAssetDumper<IW4::Material>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW4::Material>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW4::Material>* asset) override;
    };
} // namespace material
