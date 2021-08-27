#include "AssetDumperMenuList.h"

#include <filesystem>
#include <sstream>

#include "Menu/MenuDumper.h"

namespace fs = std::filesystem;

using namespace IW4;

bool AssetDumperMenuList::ShouldDump(XAssetInfo<MenuList>* asset)
{
    const auto* menuList = asset->Asset();
    const fs::path p(asset->Asset()->name);
    const auto extension = p.extension().string();

    if (extension == ".menu" && menuList->menuCount == 1)
        return false;

    return true;
}

void AssetDumperMenuList::DumpAsset(AssetDumpingContext& context, XAssetInfo<MenuList>* asset)
{
    const auto* menuList = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name);
    const fs::path p(asset->Asset()->name);

    if (!assetFile)
        return;

    std::string parentPath;
    if(p.has_parent_path())
        parentPath = p.parent_path().string() + "/";

    MenuDumper menuDumper(*assetFile);

    menuDumper.Start();

    for(auto menuNum = 0; menuNum < menuList->menuCount; menuNum++)
    {
        const auto* menu = menuList->menus[menuNum];

        std::ostringstream ss;
        ss << parentPath << menu->window.name << ".menu";
        menuDumper.IncludeMenu(ss.str());
    }

    menuDumper.End();
}
