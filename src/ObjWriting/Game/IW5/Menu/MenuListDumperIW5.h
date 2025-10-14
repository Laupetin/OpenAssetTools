#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace menu
{
    class MenuListDumperIW5 final : public AbstractAssetDumper<IW5::MenuList>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::MenuList>& asset) override;
    };
} // namespace menu
