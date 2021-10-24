#include "AssetDumperMenuDef.h"

#include <filesystem>
#include <string>

#include "ObjWriting.h"
#include "Game/IW4/GameAssetPoolIW4.h"
#include "Game/IW4/Menu/MenuDumperIW4.h"
#include "Menu/AbstractMenuDumper.h"

namespace fs = std::filesystem;

using namespace IW4;

const MenuList* AssetDumperMenuDef::GetParentMenuList(XAssetInfo<menuDef_t>* asset)
{
    const auto* menu = asset->Asset();
    const auto* gameAssetPool = dynamic_cast<GameAssetPoolIW4*>(asset->m_zone->m_pools.get());
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
    const auto assetFile = context.OpenAssetFile(GetPathForMenu(asset));

    if (!assetFile)
        return;

    MenuDumper menuDumper(*assetFile);

    menuDumper.Start();
    menuDumper.WriteMenu(menu);
    menuDumper.End();
}
