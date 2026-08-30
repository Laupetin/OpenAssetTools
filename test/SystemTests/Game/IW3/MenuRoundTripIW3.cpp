#include "Game/IW3/IW3.h"
#include "Game/IW3/Menu/MenuDumperIW3.h"
#include "Game/IW3/Menu/MenuListDumperIW3.h"
#include "Linker.h"
#include "OatTestPaths.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"
#include "SystemTestsPaths.h"
#include "ZoneLoading.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <memory>
#include <string>

namespace fs = std::filesystem;
using namespace IW3;
using namespace std::literals;

namespace
{
    TEST_CASE("Menu loading and dumping (IW3)", "[iw3][menu][system]")
    {
        const auto testDataPath = oat::paths::GetSystemTestsDirectory() / "Game/IW3/MenuRoundTrip";
        const auto testDataPathString = testDataPath.string();
        const auto outputPath = oat::paths::GetTempDirectory("MenuRoundTripIW3");
        const auto outputPathString = outputPath.string();

        const char* arguments[]{
            "SystemTests",
            "--verbose",
            "--base-folder",
            outputPathString.c_str(),
            "--asset-search-path",
            testDataPathString.c_str(),
            "--source-search-path",
            testDataPathString.c_str(),
            "--output-folder",
            outputPathString.c_str(),
            "MenuRoundTripIW3",
        };

        LinkerArgs linkerArgs;
        auto shouldContinue = true;
        REQUIRE(linkerArgs.ParseArgs(std::size(arguments), arguments, shouldContinue));
        REQUIRE(shouldContinue);

        const auto linker = Linker::Create(std::move(linkerArgs));
        REQUIRE(linker->Start());

        const auto zonePath = outputPath / "MenuRoundTripIW3.ff";
        auto maybeZone = ZoneLoading::LoadZone(zonePath.string(), std::nullopt);
        REQUIRE(maybeZone);
        auto zone = std::move(*maybeZone);

        const auto* menuList = zone->m_pools.GetAsset<AssetMenuList>("ui_mp/menus.txt");
        const auto* mainMenu = zone->m_pools.GetAsset<AssetMenu>("main");
        const auto* popupMenu = zone->m_pools.GetAsset<AssetMenu>("popup");
        REQUIRE(menuList);
        REQUIRE(mainMenu);
        REQUIRE(popupMenu);
        REQUIRE(menuList->Asset()->menuCount == 2);
        REQUIRE(mainMenu->Asset()->visibleExp.numEntries == 3);
        REQUIRE(mainMenu->Asset()->window.dynamicFlags[0] & 0x10000);
        REQUIRE(mainMenu->Asset()->items[0]->text != nullptr);
        REQUIRE(mainMenu->Asset()->items[0]->text == ""s);
        REQUIRE(mainMenu->Asset()->items[0]->window.dynamicFlags[0] & 0x10000);
        REQUIRE(mainMenu->Asset()->items[0]->action == R"("setItemColor" "empty_text" "borderColor" "0.1" "0.1" "0.12" "0.5" ; )"s);

        MockSearchPath objPath;
        MockOutputPath dumpOutput;
        AssetDumpingContext dumpingContext(*zone, "", dumpOutput, objPath, std::nullopt);
        menu::MenuListDumperIW3 menuListDumper;
        menuListDumper.Dump(dumpingContext);
        menu::MenuDumperIW3 menuDumper;
        menuDumper.Dump(dumpingContext);

        const auto* dumpedMenuList = dumpOutput.GetMockedFile("ui_mp/menus.txt");
        const auto* dumpedMainMenu = dumpOutput.GetMockedFile("ui_mp/main.menu");
        const auto* dumpedPopupMenu = dumpOutput.GetMockedFile("ui_mp/popup.menu");
        REQUIRE(dumpedMenuList);
        REQUIRE(dumpedMainMenu);
        REQUIRE(dumpedPopupMenu);
        REQUIRE(dumpedMenuList->AsString().find("loadMenu { \"ui_mp/main.menu\" }") != std::string::npos);
        REQUIRE(dumpedMenuList->AsString().find("loadMenu { \"ui_mp/popup.menu\" }") != std::string::npos);
        REQUIRE(dumpedMainMenu->AsString().find("text                        \"\"") != std::string::npos);
        REQUIRE(dumpedMainMenu->AsString().find("visible                     when(dvarbool(\"ui_show_main\"));") != std::string::npos);
        REQUIRE(dumpedMainMenu->AsString().find("setItemColor empty_text borderColor 0.1 0.1 0.12 0.5;") != std::string::npos);
    }
} // namespace
