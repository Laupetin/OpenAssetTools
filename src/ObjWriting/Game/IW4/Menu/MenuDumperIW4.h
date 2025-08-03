#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Menu/MenuDumpingZoneState.h"

namespace IW4::menu
{
    class MenuDumper final : public AbstractAssetDumper<menuDef_t>
    {
    protected:
        bool ShouldDump(XAssetInfo<menuDef_t>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<menuDef_t>* asset) override;
    };
} // namespace IW4::menu
