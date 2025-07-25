#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/Menu/MenuDumperIW4.h"
#include "Menu/MenuDumpingZoneState.h"

namespace IW4
{
    class AssetDumperMenuList final : public AbstractAssetDumper<MenuList>
    {
        static std::vector<const ExpressionSupportingData*> GetAllUniqueExpressionSupportingData(const MenuList* menuList);

        static void DumpFunctions(MenuDumper& menuDumper, const MenuList* menuList);
        static void DumpMenus(MenuDumper& menuDumper, menu::MenuDumpingZoneState* zoneState, const MenuList* menuList);

        static std::string PathForMenu(const std::string& menuListParentPath, const menuDef_t* menu);

    protected:
        bool ShouldDump(XAssetInfo<MenuList>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<MenuList>* asset) override;

    public:
        static void CreateDumpingStateForMenuList(menu::MenuDumpingZoneState* zoneState, const MenuList* menuList);
        void DumpPool(AssetDumpingContext& context, AssetPool<MenuList>* pool) override;
    };
} // namespace IW4
