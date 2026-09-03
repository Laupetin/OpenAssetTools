#include "MenuDumperT4.h"

#include "MenuListDumperT4.h"
#include "MenuWriterT4.h"
#include "ObjWriting.h"

#include <format>
#include <string>

using namespace T4;

namespace
{
    std::string GetPathForMenu(menu::MenuDumpingZoneState* zoneState, const XAssetInfo<menuDef_t>& asset)
    {
        const auto menuDumpingState = zoneState->m_menu_dumping_state_map.find(asset.Asset());

        if (menuDumpingState == zoneState->m_menu_dumping_state_map.end())
            return std::format("ui_mp/{}.menu", asset.Asset()->window.name);

        return menuDumpingState->second.m_path;
    }
} // namespace

namespace menu
{
    void MenuDumperT4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetMenu::Type>& asset)
    {
        const auto* menu = asset.Asset();
        auto* zoneState = context.GetZoneAssetDumperState<MenuDumpingZoneState>();

        if (!ObjWriting::ShouldHandleAssetType(ASSET_TYPE_MENULIST))
        {
            // Make sure menu paths based on menu lists are created
            const auto menuListAssets = context.m_zone.m_pools.PoolAssets<AssetMenuList>();
            for (auto* menuListAsset : menuListAssets)
                CreateDumpingStateForMenuListT4(zoneState, menuListAsset->Asset());
        }

        const auto menuFilePath = GetPathForMenu(zoneState, asset);
        const auto assetFile = context.OpenAssetFile(menuFilePath);

        if (!assetFile)
            return;

        const auto menuWriter = CreateMenuWriterT4(*assetFile);

        menuWriter->Start();
        menuWriter->WriteMenu(*menu);
        menuWriter->End();
    }
} // namespace menu
