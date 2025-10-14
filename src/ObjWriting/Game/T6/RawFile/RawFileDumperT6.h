#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace raw_file
{
    class DumperT6 final : public AbstractAssetDumper<T6::RawFile>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::RawFile>& asset) override;
    };
} // namespace raw_file
