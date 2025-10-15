#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace raw_file
{
    class DumperT5 final : public AbstractAssetDumper<T5::AssetRawFile>
    {
    public:
        explicit DumperT5(const AssetPool<T5::AssetRawFile::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetRawFile::Type>& asset) override;
    };
} // namespace raw_file
