#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace destructible_def
{
    class InfoStringDumperT5 final : public AbstractAssetDumper<T5::AssetDestructibleDef>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetDestructibleDef::Type>& asset) override;
    };
} // namespace destructible_def
