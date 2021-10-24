#include "AssetDumperMenuDef.h"

#include <filesystem>
#include <string>

#include "ObjWriting.h"
#include "Game/IW5/GameAssetPoolIW5.h"
#include "Game/IW5/Menu/MenuDumperIW5.h"
#include "Menu/AbstractMenuDumper.h"

namespace fs = std::filesystem;

using namespace IW5;

const MenuList* AssetDumperMenuDef::GetParentMenuList(XAssetInfo<menuDef_t>* asset)
{
    const auto* menu = asset->Asset();
    const auto* gameAssetPool = dynamic_cast<GameAssetPoolIW5*>(asset->m_zone->m_pools.get());
    for (const auto* menuList : *gameAssetPool->m_menu_list)
    {
        const auto* menuListAsset = menuList->Asset();

        for (auto menuIndex = 0; menuIndex < menuListAsset->menuCount; menuIndex++)
        {
            if (menuListAsset->menus[menuIndex] == menu)
                return menuListAsset;
        }
    }

    return nullptr;
}

std::string AssetDumperMenuDef::GetPathForMenu(XAssetInfo<menuDef_t>* asset)
{
    const auto* list = GetParentMenuList(asset);

    if (!list)
        return "ui_mp/" + std::string(asset->Asset()->window.name) + ".menu";

    const fs::path p(list->name);
    std::string parentPath;
    if (p.has_parent_path())
        parentPath = p.parent_path().string() + "/";

    return parentPath + std::string(asset->Asset()->window.name) + ".menu";
}

bool AssetDumperMenuDef::ShouldDump(XAssetInfo<menuDef_t>* asset)
{
    return true;
}

void AssetDumperMenuDef::DumpAsset(AssetDumpingContext& context, XAssetInfo<menuDef_t>* asset)
{
    const auto* menu = asset->Asset();
    const auto menuFilePath = GetPathForMenu(asset);

    if(ObjWriting::ShouldHandleAssetType(ASSET_TYPE_MENULIST))
    {
        // Don't dump menu file separately if the name matches the menu list
        const auto* menuListParent = GetParentMenuList(asset);
        if (menuListParent && menuFilePath == menuListParent->name)
            return;
    }

    const auto assetFile = context.OpenAssetFile(menuFilePath);

    if (!assetFile)
        return;

    MenuDumper menuDumper(*assetFile);

    menuDumper.Start();
    menuDumper.WriteMenu(menu);
    menuDumper.End();
}
