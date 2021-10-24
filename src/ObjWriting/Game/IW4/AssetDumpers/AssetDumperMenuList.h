#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/Menu/MenuDumperIW4.h"

namespace IW4
{
    class AssetDumperMenuList final : public AbstractAssetDumper<MenuList>
    {
        static std::vector<const ExpressionSupportingData*> GetAllUniqueExpressionSupportingData(const MenuList* menuList);

        static void DumpFunctions(MenuDumper& menuDumper, const MenuList* menuList);
        static void DumpMenus(MenuDumper& menuDumper, const MenuList* menuList);

    protected:
        bool ShouldDump(XAssetInfo<MenuList>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<MenuList>* asset) override;
    };
}
