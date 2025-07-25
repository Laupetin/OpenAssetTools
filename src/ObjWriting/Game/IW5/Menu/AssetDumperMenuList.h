#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"
#include "Game/IW5/Menu/MenuDumperIW5.h"

namespace IW5
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
} // namespace IW5
