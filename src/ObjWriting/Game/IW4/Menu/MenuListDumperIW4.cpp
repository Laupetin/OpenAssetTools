#include "MenuListDumperIW4.h"

#include "Game/IW4/Menu/MenuDumperIW4.h"
#include "Menu/AbstractMenuWriter.h"
#include "MenuWriterIW4.h"
#include "ObjWriting.h"

#include <cassert>
#include <filesystem>
#include <set>
#include <sstream>

namespace fs = std::filesystem;

using namespace IW4;

namespace
{
    std::vector<const ExpressionSupportingData*> GetAllUniqueExpressionSupportingData(const MenuList* menuList)
    {
        std::vector<const ExpressionSupportingData*> result;
        std::set<const ExpressionSupportingData*> alreadyAddedSupportingData;

        if (menuList->menus == nullptr)
            return result;

        for (auto i = 0; i < menuList->menuCount; i++)
        {
            if (menuList->menus[i] == nullptr)
                continue;

            const auto* menu = menuList->menus[i];

            if (menu->expressionData == nullptr)
                continue;

            if (alreadyAddedSupportingData.find(menu->expressionData) == alreadyAddedSupportingData.end())
            {
                result.push_back(menu->expressionData);
                alreadyAddedSupportingData.emplace(menu->expressionData);
            }
        }

        return result;
    }

    void DumpFunctions(menu::IWriterIW4& menuDumper, const MenuList* menuList)
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
                if (function != nullptr)
                {
                    std::stringstream ss;
                    ss << "FUNC_" << functionIndex;

                    menuDumper.WriteFunctionDef(ss.str(), function);
                }

                functionIndex++;
            }
        }
    }

    void DumpMenus(menu::IWriterIW4& menuDumper, menu::MenuDumpingZoneState* zoneState, const MenuList* menuList)
    {
        for (auto menuNum = 0; menuNum < menuList->menuCount; menuNum++)
        {
            const auto* menu = menuList->menus[menuNum];

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
            return "";

        if (menuAssetName[0] == ',')
            menuAssetName = &menuAssetName[1];

        std::ostringstream ss;
        ss << menuListParentPath << menuAssetName << ".menu";

        return ss.str();
    }
} // namespace

namespace menu
{
    void CreateDumpingStateForMenuListIW4(MenuDumpingZoneState* zoneState, const MenuList* menuList)
    {
        if (menuList->menuCount <= 0 || menuList->menus == nullptr || menuList->name == nullptr)
            return;

        const std::string menuListName(menuList->name);
        const fs::path p(menuListName);
        std::string parentPath;
        if (p.has_parent_path())
            parentPath = p.parent_path().string() + "/";

        for (auto i = 0; i < menuList->menuCount; i++)
        {
            auto* menu = menuList->menus[i];

            if (menu == nullptr)
                continue;

            auto existingState = zoneState->m_menu_dumping_state_map.find(menu);
            if (existingState == zoneState->m_menu_dumping_state_map.end())
            {
                auto menuPath = PathForMenu(parentPath, menu);
                const auto isTheSameAsMenuList = menuPath == menuListName;
                zoneState->CreateMenuDumpingState(menu, std::move(menuPath), isTheSameAsMenuList ? menuList : nullptr);
            }
            else if (existingState->second.m_alias_menu_list == nullptr)
            {
                auto menuPath = PathForMenu(parentPath, menu);
                const auto isTheSameAsMenuList = menuPath == menuListName;
                if (isTheSameAsMenuList)
                {
                    existingState->second.m_alias_menu_list = menuList;
                    existingState->second.m_path = std::move(menuPath);
                }
            }
        }
    }

    bool MenuListDumperIW4::ShouldDump(XAssetInfo<MenuList>* asset)
    {
        return true;
    }

    void MenuListDumperIW4::DumpAsset(AssetDumpingContext& context, XAssetInfo<MenuList>* asset)
    {
        const auto* menuList = asset->Asset();
        const auto assetFile = context.OpenAssetFile(asset->m_name);

        if (!assetFile)
            return;

        auto* zoneState = context.GetZoneAssetDumperState<MenuDumpingZoneState>();

        auto menuWriter = CreateMenuWriterIW4(*assetFile);

        menuWriter->Start();

        if (!ObjWriting::Configuration.MenuLegacyMode)
            DumpFunctions(*menuWriter, menuList);

        DumpMenus(*menuWriter, zoneState, menuList);

        menuWriter->End();
    }

    void MenuListDumperIW4::DumpPool(AssetDumpingContext& context, AssetPool<MenuList>* pool)
    {
        auto* zoneState = context.GetZoneAssetDumperState<MenuDumpingZoneState>();

        for (auto* asset : *pool)
            CreateDumpingStateForMenuListIW4(zoneState, asset->Asset());

        AbstractAssetDumper<MenuList>::DumpPool(context, pool);
    }
} // namespace menu
