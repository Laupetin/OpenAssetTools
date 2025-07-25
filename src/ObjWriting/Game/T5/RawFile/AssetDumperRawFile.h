#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace T5
{
    class AssetDumperRawFile final : public AbstractAssetDumper<RawFile>
    {
        constexpr static size_t GSC_MAX_SIZE = 0xC000000;

        static void DumpGsc(AssetDumpingContext& context, XAssetInfo<RawFile>* asset, std::ostream& stream);

    protected:
        bool ShouldDump(XAssetInfo<RawFile>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<RawFile>* asset) override;
    };
} // namespace T5
