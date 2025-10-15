#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace string_table
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::AssetStringTable>
    {
    public:
        explicit DumperIW3(const AssetPool<IW3::AssetStringTable::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::StringTable>& asset) override;
    };
} // namespace string_table
