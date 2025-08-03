#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace IW5::menu
{
    class MenuListDumper final : public AbstractAssetDumper<MenuList>
    {
    protected:
        bool ShouldDump(XAssetInfo<MenuList>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<MenuList>* asset) override;
    };
} // namespace IW5::menu
