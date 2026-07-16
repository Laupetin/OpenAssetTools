#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace menu
{
    class MenuDumperIW3 final : public AbstractAssetDumper<IW3::AssetMenu>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetMenu::Type>& asset) override;
    };
} // namespace menu
