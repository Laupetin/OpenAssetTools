#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace material
{
    class DecompilingGdtDumperIW4 final : public AbstractAssetDumper<IW4::Material>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::Material>& asset) override;
    };
} // namespace material
