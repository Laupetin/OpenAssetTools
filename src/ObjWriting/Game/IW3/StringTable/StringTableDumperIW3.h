#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace string_table
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::StringTable>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW3::StringTable>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW3::StringTable>* asset) override;
    };
} // namespace string_table
