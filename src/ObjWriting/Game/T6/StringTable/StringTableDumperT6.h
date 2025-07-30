#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6::string_table
{
    class Dumper final : public AbstractAssetDumper<StringTable>
    {
    protected:
        bool ShouldDump(XAssetInfo<StringTable>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<StringTable>* asset) override;
    };
} // namespace T6::string_table
