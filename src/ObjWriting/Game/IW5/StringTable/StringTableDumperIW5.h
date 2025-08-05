#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace string_table
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::StringTable>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW5::StringTable>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW5::StringTable>* asset) override;
    };
} // namespace string_table
