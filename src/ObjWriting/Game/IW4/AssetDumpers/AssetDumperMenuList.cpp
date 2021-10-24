#include "AssetDumperMenuList.h"

#include <cassert>
#include <filesystem>
#include <sstream>
#include <set>

#include "ObjWriting.h"
#include "Game/IW4/Menu/MenuDumperIW4.h"
#include "Menu/AbstractMenuDumper.h"

namespace fs = std::filesystem;

using namespace IW4;

std::vector<const ExpressionSupportingData*> AssetDumperMenuList::GetAllUniqueExpressionSupportingData(const MenuList* menuList)
{
    std::vector<const ExpressionSupportingData*> result;
    std::set<const ExpressionSupportingData*> alreadyAddedSupportingData;

    if (menuList->menus == nullptr)
        return result;

    for(auto i = 0; i < menuList->menuCount; i++)
    {
        if(menuList->menus[i] == nullptr)
            continue;

        const auto* menu = menuList->menus[i];

        if(menu->expressionData == nullptr)
            continue;

        if(alreadyAddedSupportingData.find(menu->expressionData) == alreadyAddedSupportingData.end())
        {
            result.push_back(menu->expressionData);
            alreadyAddedSupportingData.emplace(menu->expressionData);
        }
    }

    return result;
}

void AssetDumperMenuList::DumpFunctions(MenuDumper& menuDumper, const MenuList* menuList)
{
    const auto allSupportingData = GetAllUniqueExpressionSupportingData(menuList);
    auto functionIndex = 0u;

    assert(allSupportingData.size() <= 1);

    for (const auto* supportingData : allSupportingData)
    {
        if (supportingData->uifunctions.functions == nullptr)
            continue;

        for(auto i = 0; i < supportingData->uifunctions.totalFunctions; i++)
        {
            const auto* function = supportingData->uifunctions.functions[i];
            if(function == nullptr)
                continue;

            std::stringstream ss;
            ss << "FUNC_" << functionIndex;

            menuDumper.WriteFunctionDef(ss.str(), function);

            functionIndex++;
        }
    }
}

void AssetDumperMenuList::DumpMenus(MenuDumper& menuDumper, const MenuList* menuList)
{
    const fs::path p(menuList->name);

    std::string parentPath;
    if (p.has_parent_path())
        parentPath = p.parent_path().string() + "/";

    for (auto menuNum = 0; menuNum < menuList->menuCount; menuNum++)
    {
        const auto* menu = menuList->menus[menuNum];
        const auto* menuAssetName = menu->window.name;
        if (menuAssetName && menuAssetName[0] == ',')
            menuAssetName = &menuAssetName[1];

        std::ostringstream ss;
        ss << parentPath << menuAssetName << ".menu";

        const auto menuName = ss.str();

        // If the menu was embedded directly as menu list write its data in the menu list file
        if (menuName == menuList->name)
            menuDumper.WriteMenu(menu);
        else
            menuDumper.IncludeMenu(ss.str());
    }
}

bool AssetDumperMenuList::ShouldDump(XAssetInfo<MenuList>* asset)
{
    return true;
}

void AssetDumperMenuList::DumpAsset(AssetDumpingContext& context, XAssetInfo<MenuList>* asset)
{
    const auto* menuList = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name);

    if (!assetFile)
        return;

    MenuDumper menuDumper(*assetFile);

    menuDumper.Start();

    if(!ObjWriting::Configuration.MenuLegacyMode)
        DumpFunctions(menuDumper, menuList);

    DumpMenus(menuDumper, menuList);

    menuDumper.End();
}
