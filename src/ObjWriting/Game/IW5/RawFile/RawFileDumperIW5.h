#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace raw_file
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AssetRawFile>
    {
    public:
        explicit DumperIW5(const AssetPool<IW5::AssetRawFile::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetRawFile::Type>& asset) override;
    };
} // namespace raw_file
