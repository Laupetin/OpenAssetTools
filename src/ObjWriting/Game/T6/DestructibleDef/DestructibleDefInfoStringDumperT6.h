#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace destructible_def
{
    class InfoStringDumperT6 final : public AbstractAssetDumper<T6::AssetDestructibleDef>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetDestructibleDef::Type>& asset) override;
    };
} // namespace destructible_def
