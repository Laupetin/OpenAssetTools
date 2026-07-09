#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/QOS/QOS.h"

namespace string_table
{
    class DumperQOS final : public AbstractAssetDumper<QOS::AssetStringTable>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<QOS::StringTable>& asset) override;
    };
} // namespace string_table
