#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace xanim
{
    class DumperT5 final : public AbstractAssetDumper<T5::AssetXAnim>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetXAnim::Type>& asset) override;
    };
} // namespace xanim
