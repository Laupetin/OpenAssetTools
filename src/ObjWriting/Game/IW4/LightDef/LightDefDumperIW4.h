#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace light_def
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetLightDef>
    {
    public:
        explicit DumperIW4(const AssetPool<IW4::AssetLightDef::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetLightDef::Type>& asset) override;
    };
} // namespace light_def
