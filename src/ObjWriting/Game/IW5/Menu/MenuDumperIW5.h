#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"
#include "Game/IW5/Menu/MenuDumperIW5.h"

namespace menu
{
    class MenuDumperIW5 final : public AbstractAssetDumper<IW5::menuDef_t>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::menuDef_t>& asset) override;
    };
} // namespace menu
