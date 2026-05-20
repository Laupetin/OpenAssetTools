#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW6/IW6.h"

namespace menu
{
    class MenuDumperIW6 final : public AbstractAssetDumper<IW6::AssetMenu>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW6::AssetMenu::Type>& asset) override;
    };
} // namespace menu
