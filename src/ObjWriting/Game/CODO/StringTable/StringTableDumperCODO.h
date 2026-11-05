#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/CODO/CODO.h"

namespace string_table
{
    class DumperCODO final : public AbstractAssetDumper<CODO::AssetStringTable>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<CODO::AssetStringTable::Type>& asset) override;
    };
} // namespace string_table
