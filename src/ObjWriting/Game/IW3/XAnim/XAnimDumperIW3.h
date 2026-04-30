#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace xanim
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::AssetXAnim>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetXAnim::Type>& asset) override;
    };
} // namespace xanim
