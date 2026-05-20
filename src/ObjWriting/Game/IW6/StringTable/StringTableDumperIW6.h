#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW6/IW6.h"

namespace string_table
{
    class DumperIW6 final : public AbstractAssetDumper<IW6::AssetStringTable>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW6::AssetStringTable::Type>& asset) override;
    };
} // namespace string_table
