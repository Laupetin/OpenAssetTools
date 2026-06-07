#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T4/T4.h"

namespace raw_file
{
    class DumperT4 final : public AbstractAssetDumper<T4::AssetRawFile>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T4::RawFile>& asset) override;
    };
} // namespace raw_file
