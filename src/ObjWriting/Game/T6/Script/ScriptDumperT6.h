#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace script
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetScript>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetScript::Type>& asset) override;
    };
} // namespace script
