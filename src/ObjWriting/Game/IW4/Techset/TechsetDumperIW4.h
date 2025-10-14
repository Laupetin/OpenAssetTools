#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace techset
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::MaterialTechniqueSet>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::MaterialTechniqueSet>& asset) override;
    };
} // namespace techset
