#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace string_table
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetStringTable>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetStringTable::Type>& asset) override;
    };
} // namespace string_table
