#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace menu
{
    class MenuDumperIW4 final : public AbstractAssetDumper<IW4::AssetMenu>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetMenu::Type>& asset) override;
    };
} // namespace menu
