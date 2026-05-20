#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW6/IW6.h"

namespace menu
{
    class MenuListDumperIW6 final : public AbstractAssetDumper<IW6::AssetMenuList>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW6::AssetMenuList::Type>& asset) override;
    };
} // namespace menu
