#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"
#include "Game/IW5/Menu/MenuDumperIW5.h"

namespace IW5::menu
{
    class MenuDumper final : public AbstractAssetDumper<menuDef_t>
    {
    protected:
        bool ShouldDump(XAssetInfo<menuDef_t>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<menuDef_t>* asset) override;
    };
} // namespace IW5::menu
