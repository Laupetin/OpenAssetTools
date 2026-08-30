#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T4/T4.h"

namespace menu
{
    class MenuDumperT4 final : public AbstractAssetDumper<T4::AssetMenu>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T4::AssetMenu::Type>& asset) override;
    };
} // namespace menu
