#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace menu
{
    class MenuDumperIW5 final : public AbstractAssetDumper<IW5::AssetMenu>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetMenu::Type>& asset) override;
    };
} // namespace menu
