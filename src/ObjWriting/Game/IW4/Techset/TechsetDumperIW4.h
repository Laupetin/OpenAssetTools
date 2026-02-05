#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace techset
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetTechniqueSet>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetTechniqueSet::Type>& asset) override;
    };
} // namespace techset
