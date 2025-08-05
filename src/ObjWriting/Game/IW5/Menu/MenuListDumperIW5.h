#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace menu
{
    class MenuListDumperIW5 final : public AbstractAssetDumper<IW5::MenuList>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW5::MenuList>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW5::MenuList>* asset) override;
    };
} // namespace menu
