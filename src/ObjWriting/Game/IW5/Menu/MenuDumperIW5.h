#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"
#include "Game/IW5/Menu/MenuDumperIW5.h"

namespace menu
{
    class MenuDumperIW5 final : public AbstractAssetDumper<IW5::menuDef_t>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW5::menuDef_t>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW5::menuDef_t>* asset) override;
    };
} // namespace menu
