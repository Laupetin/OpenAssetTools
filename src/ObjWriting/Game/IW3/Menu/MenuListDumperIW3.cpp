#include "MenuListDumperIW3.h"

#include "MenuWriterIW3.h"

#include <filesystem>
#include <format>

namespace fs = std::filesystem;

using namespace IW3;

namespace
{
    void DumpMenus(menu::IWriterIW3& menuDumper, menu::MenuDumpingZoneState* zoneState, const MenuList* menuList)
    {
        if (!menuList->menus)
            return;

        for (auto menuNum = 0; menuNum < menuList->menuCount; menuNum++)
        {
            const auto* menu = menuList->menus[menuNum];
            if (!menu)
                continue;

            const auto menuDumpingState = zoneState->m_menu_dumping_state_map.find(menu);
            if (menuDumpingState == zoneState->m_menu_dumping_state_map.end())
                continue;

            // If the menu was embedded directly as menu list write its data in the menu list file
            if (menuDumpingState->second.m_alias_menu_list == menuList)
                menuDumper.WriteMenu(*menu);
            else
                menuDumper.IncludeMenu(menuDumpingState->second.m_path);
        }
    }

    std::string PathForMenu(const std::string& menuListParentPath, const menuDef_t* menu)
    {
        const auto* menuAssetName = menu->window.name;

        if (!menuAssetName)
            return {};

        if (menuAssetName[0] == ',')
            menuAssetName = &menuAssetName[1];

        return std::format("{}{}.menu", menuListParentPath, menuAssetName);
    }
} // namespace

namespace menu
{
    void CreateDumpingStateForMenuListIW3(MenuDumpingZoneState* zoneState, const MenuList* menuList)
    {
        if (!menuList || menuList->menuCount <= 0 || !menuList->menus || !menuList->name)
            return;

        const std::string menuListName(menuList->name);
        const fs::path p(menuListName);
        std::string parentPath;
        if (p.has_parent_path())
            parentPath = p.parent_path().generic_string() + "/";

        for (auto i = 0; i < menuList->menuCount; i++)
        {
            const auto* menu = menuList->menus[i];

            if (!menu)
                continue;

            auto menuPath = PathForMenu(parentPath, menu);
            if (menuPath.empty())
                continue;

            auto existingState = zoneState->m_menu_dumping_state_map.find(menu);
            if (existingState == zoneState->m_menu_dumping_state_map.end())
            {
                const auto isTheSameAsMenuList = menuPath == menuListName;
                zoneState->CreateMenuDumpingState(menu, std::move(menuPath), isTheSameAsMenuList ? menuList : nullptr);
            }
            else if (!existingState->second.m_alias_menu_list)
            {
                const auto isTheSameAsMenuList = menuPath == menuListName;
                if (isTheSameAsMenuList)
                {
                    existingState->second.m_alias_menu_list = menuList;
                    existingState->second.m_path = std::move(menuPath);
                }
            }
        }
    }

    void MenuListDumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetMenuList::Type>& asset)
    {
        const auto* menuList = asset.Asset();
        const auto assetFile = context.OpenAssetFile(asset.m_name);

        if (!assetFile)
            return;

        auto* zoneState = context.GetZoneAssetDumperState<MenuDumpingZoneState>();

        const auto menuWriter = CreateMenuWriterIW3(*assetFile);

        menuWriter->Start();
        DumpMenus(*menuWriter, zoneState, menuList);
        menuWriter->End();
    }

    void MenuListDumperIW3::Dump(AssetDumpingContext& context)
    {
        auto* zoneState = context.GetZoneAssetDumperState<MenuDumpingZoneState>();

        auto menuListAssets = context.m_zone.m_pools.PoolAssets<AssetMenuList>();
        for (const auto* asset : menuListAssets)
            CreateDumpingStateForMenuListIW3(zoneState, asset->Asset());

        AbstractAssetDumper::Dump(context);
    }
} // namespace menu
