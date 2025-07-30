#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace T5::string_table
{
    class Dumper final : public AbstractAssetDumper<StringTable>
    {
    protected:
        bool ShouldDump(XAssetInfo<StringTable>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<StringTable>* asset) override;
    };
} // namespace T5::string_table
