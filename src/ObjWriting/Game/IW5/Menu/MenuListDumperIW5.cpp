#include "MenuListDumperIW5.h"

#include "Game/IW5/Menu/MenuWriterIW5.h"
#include "Menu/AbstractMenuWriter.h"
#include "MenuWriterIW5.h"
#include "ObjWriting.h"

#include <cassert>
#include <filesystem>
#include <format>
#include <unordered_set>

namespace fs = std::filesystem;

using namespace IW5;

namespace
{
    std::vector<const ExpressionSupportingData*> GetAllUniqueExpressionSupportingData(const MenuList* menuList)
    {
        std::vector<const ExpressionSupportingData*> result;
        std::unordered_set<const ExpressionSupportingData*> alreadyAddedSupportingData;

        if (!menuList->menus)
            return result;

        for (auto i = 0; i < menuList->menuCount; i++)
        {
            if (!menuList->menus[i])
                continue;

            const auto* menu = menuList->menus[i];

            if (!menu->data || !menu->data->expressionData)
                continue;

            if (!alreadyAddedSupportingData.contains(menu->data->expressionData))
            {
                result.emplace_back(menu->data->expressionData);
                alreadyAddedSupportingData.emplace(menu->data->expressionData);
            }
        }

        return result;
    }

    void DumpFunctions(menu::IWriterIW5& menuDumper, const MenuList* menuList)
    {
        const auto allSupportingData = GetAllUniqueExpressionSupportingData(menuList);
        auto functionIndex = 0u;

        assert(allSupportingData.size() <= 1);

        for (const auto* supportingData : allSupportingData)
        {
            if (supportingData->uifunctions.functions == nullptr)
                continue;

            for (auto i = 0; i < supportingData->uifunctions.totalFunctions; i++)
            {
                const auto* function = supportingData->uifunctions.functions[i];
                if (!function)
                    continue;

                menuDumper.WriteFunctionDef(std::format("FUNC_{}", functionIndex), function);

                functionIndex++;
            }
        }
    }

    void DumpMenus(menu::IWriterIW5& menuDumper, menu::MenuDumpingZoneState* zoneState, const MenuList* menuList)
    {
        for (auto menuNum = 0; menuNum < menuList->menuCount; menuNum++)
        {
            const auto* menu = menuList->menus[menuNum];
            if (!menu)
                continue;

            const auto menuDumpingState = zoneState->m_menu_dumping_state_map.find(menu);
            if (menuDumpingState == zoneState->m_menu_dumping_state_map.end())
                continue;

            const auto* menuAssetName = menu->window.name;
            const auto isReference = menuAssetName && menuAssetName[0] == ',';

            // If the menu was embedded directly as menu list write its data in the menu list file
            if (!isReference && menuDumpingState->second.m_alias_menu_list == menuList)
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
    void CreateDumpingStateForMenuListIW5(MenuDumpingZoneState* zoneState, const MenuList* menuList)
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

    void MenuListDumperIW5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetMenuList::Type>& asset)
    {
        const auto* menuList = asset.Asset();
        const auto assetFile = context.OpenAssetFile(asset.m_name);

        if (!assetFile)
            return;

        auto* zoneState = context.GetZoneAssetDumperState<MenuDumpingZoneState>();

        const auto menuWriter = CreateMenuWriterIW5(*assetFile);

        menuWriter->Start();
        if (!ObjWriting::Configuration.MenuLegacyMode)
            DumpFunctions(*menuWriter, menuList);
        DumpMenus(*menuWriter, zoneState, menuList);
        menuWriter->End();
    }

    void MenuListDumperIW5::Dump(AssetDumpingContext& context)
    {
        auto* zoneState = context.GetZoneAssetDumperState<MenuDumpingZoneState>();

        const auto menuListAssets = context.m_zone.m_pools.PoolAssets<AssetMenuList>();
        for (const auto* asset : menuListAssets)
            CreateDumpingStateForMenuListIW5(zoneState, asset->Asset());

        AbstractAssetDumper::Dump(context);
    }
} // namespace menu
