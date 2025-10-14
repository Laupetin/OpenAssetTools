#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace string_table
{
    class DumperT5 final : public AbstractAssetDumper<T5::StringTable>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::StringTable>& asset) override;
    };
} // namespace string_table
