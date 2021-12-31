#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Menu/MenuDumpingZoneState.h"

namespace IW4
{
    class AssetDumperMenuDef final : public AbstractAssetDumper<menuDef_t>
    {
        static std::string GetPathForMenu(menu::MenuDumpingZoneState* zoneState, XAssetInfo<menuDef_t>* asset);

    protected:
        bool ShouldDump(XAssetInfo<menuDef_t>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<menuDef_t>* asset) override;
    };
}
