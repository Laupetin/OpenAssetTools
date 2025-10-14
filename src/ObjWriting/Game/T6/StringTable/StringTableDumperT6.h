#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace string_table
{
    class DumperT6 final : public AbstractAssetDumper<T6::StringTable>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::StringTable>& asset) override;
    };
} // namespace string_table
