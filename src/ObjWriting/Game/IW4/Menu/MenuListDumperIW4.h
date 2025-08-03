#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/Menu/MenuDumperIW4.h"
#include "Menu/MenuDumpingZoneState.h"

namespace IW4::menu
{
    void CreateDumpingStateForMenuList(::menu::MenuDumpingZoneState* zoneState, const MenuList* menuList);

    class MenuListDumper final : public AbstractAssetDumper<MenuList>
    {
    protected:
        bool ShouldDump(XAssetInfo<MenuList>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<MenuList>* asset) override;

    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<MenuList>* pool) override;
    };
} // namespace IW4::menu
