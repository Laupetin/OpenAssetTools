#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/QOS/QOS.h"

namespace raw_file
{
    class DumperQOS final : public AbstractAssetDumper<QOS::AssetRawFile>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<QOS::RawFile>& asset) override;
    };
} // namespace raw_file
