#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Menu/MenuDumpingZoneState.h"

namespace menu
{
    class MenuDumperIW4 final : public AbstractAssetDumper<IW4::menuDef_t>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW4::menuDef_t>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW4::menuDef_t>* asset) override;
    };
} // namespace menu
