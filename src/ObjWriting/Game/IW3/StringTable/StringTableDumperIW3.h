#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace IW3::string_table
{
    class Dumper final : public AbstractAssetDumper<StringTable>
    {
    protected:
        bool ShouldDump(XAssetInfo<StringTable>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<StringTable>* asset) override;
    };
} // namespace IW3
