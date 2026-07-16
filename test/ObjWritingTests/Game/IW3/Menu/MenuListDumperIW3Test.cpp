#include "Game/IW3/Menu/MenuListDumperIW3.h"

#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>
#include <iterator>
#include <memory>

using namespace IW3;

namespace
{
    TEST_CASE("MenuListDumperIW3: Can dump several menu entries", "[iw3][menu][menulist][assetdumper]")
    {
        menuDef_t mainMenu{};
        mainMenu.window.name = "main";

        menuDef_t optionsMenu{};
        optionsMenu.window.name = "options";

        menuDef_t confirmationMenu{};
        confirmationMenu.window.name = ",confirmation";

        menuDef_t* menus[]{&mainMenu, &optionsMenu, &confirmationMenu};

        MenuList menuList{};
        menuList.name = "ui_mp/menus.txt";
        menuList.menuCount = static_cast<int>(std::size(menus));
        menuList.menus = menus;

        Zone zone("MockZone", 0, GameId::IW3, GamePlatform::PC);
        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<MenuList>>(ASSET_TYPE_MENULIST, menuList.name, &menuList));

        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        menu::MenuListDumperIW3 dumper;
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("ui_mp/menus.txt");
        REQUIRE(file);

        constexpr auto expectedOutput = R"({
    loadMenu { "ui_mp/main.menu" }
    loadMenu { "ui_mp/options.menu" }
    loadMenu { "ui_mp/confirmation.menu" }
}
)";
        REQUIRE(file->AsString() == expectedOutput);
    }
} // namespace
