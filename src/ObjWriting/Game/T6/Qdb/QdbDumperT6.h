#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace qdb
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetQdb>
    {
    public:
        explicit DumperT6(const AssetPool<T6::AssetQdb::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetQdb::Type>& asset) override;
    };
} // namespace qdb
