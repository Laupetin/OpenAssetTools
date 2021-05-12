#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace T5
{
    class AssetDumperRawFile final : public AbstractAssetDumper<RawFile>
    {
        constexpr static size_t GSC_MAX_SIZE = 0xC000000;

        void DumpGsc(AssetDumpingContext& context, XAssetInfo<RawFile>* asset, std::ostream& stream);

    protected:
        bool ShouldDump(XAssetInfo<RawFile>* asset) override;
        bool CanDumpAsRaw() override;

        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<RawFile>* asset) override;
        void DumpRaw(AssetDumpingContext& context, XAssetInfo<RawFile>* asset, std::ostream& stream) override;
    };
}
