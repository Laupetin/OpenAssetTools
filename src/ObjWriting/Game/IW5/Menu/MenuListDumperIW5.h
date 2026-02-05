#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace menu
{
    class MenuListDumperIW5 final : public AbstractAssetDumper<IW5::AssetMenuList>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetMenuList::Type>& asset) override;
    };
} // namespace menu
