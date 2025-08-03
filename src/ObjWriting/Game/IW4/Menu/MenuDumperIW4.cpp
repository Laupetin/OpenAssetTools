#include "MenuDumperIW4.h"

#include "Game/IW4/GameAssetPoolIW4.h"
#include "Game/IW4/Menu/MenuDumperIW4.h"
#include "MenuListDumperIW4.h"
#include "MenuWriterIW4.h"
#include "ObjWriting.h"

#include <filesystem>
#include <string>

using namespace IW4;
using namespace ::menu;

namespace
{
    std::string GetPathForMenu(MenuDumpingZoneState* zoneState, XAssetInfo<menuDef_t>* asset)
    {
        const auto menuDumpingState = zoneState->m_menu_dumping_state_map.find(asset->Asset());

        if (menuDumpingState == zoneState->m_menu_dumping_state_map.end())
            return "ui_mp/" + std::string(asset->Asset()->window.name) + ".menu";

        return menuDumpingState->second.m_path;
    }
} // namespace

namespace IW4::menu
{
    bool MenuDumper::ShouldDump(XAssetInfo<menuDef_t>* asset)
    {
        return true;
    }

    void MenuDumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<menuDef_t>* asset)
    {
        const auto* menu = asset->Asset();
        auto* zoneState = context.GetZoneAssetDumperState<MenuDumpingZoneState>();

        if (!ObjWriting::ShouldHandleAssetType(ASSET_TYPE_MENULIST))
        {
            // Make sure menu paths based on menu lists are created
            const auto* gameAssetPool = dynamic_cast<GameAssetPoolIW4*>(asset->m_zone->m_pools.get());
            for (auto* menuListAsset : *gameAssetPool->m_menu_list)
                CreateDumpingStateForMenuList(zoneState, menuListAsset->Asset());
        }

        const auto menuFilePath = GetPathForMenu(zoneState, asset);
        const auto assetFile = context.OpenAssetFile(menuFilePath);

        if (!assetFile)
            return;

        auto menuWriter = CreateMenuWriter(*assetFile);

        menuWriter->Start();
        menuWriter->WriteMenu(*menu);
        menuWriter->End();
    }
} // namespace IW4::menu
