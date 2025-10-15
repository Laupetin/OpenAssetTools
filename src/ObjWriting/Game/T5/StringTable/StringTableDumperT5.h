#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace string_table
{
    class DumperT5 final : public AbstractAssetDumper<T5::AssetStringTable>
    {
    public:
        explicit DumperT5(const AssetPool<T5::AssetStringTable::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetStringTable::Type>& asset) override;
    };
} // namespace string_table
