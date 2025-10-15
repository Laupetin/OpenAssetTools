#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace string_table
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetStringTable>
    {
    public:
        explicit DumperT6(const AssetPool<T6::AssetStringTable::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetStringTable::Type>& asset) override;
    };
} // namespace string_table
