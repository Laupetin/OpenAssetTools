#include "Game/IW5/Menu/LoaderMenuListIW5.h"

#include "ObjLoading.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <string>

using namespace IW5;

namespace test::game::iw5::menu::parsing::it
{
    struct RestoreMenuLoadingConfiguration
    {
        ObjLoading::Configuration_t original{ObjLoading::Configuration};

        ~RestoreMenuLoadingConfiguration()
        {
            ObjLoading::Configuration = original;
        }
    };

    TEST_CASE("MenuParsingIW5IT: Numeric text expressions respect permissive mode", "[parsing][converting][menu][it]")
    {
        RestoreMenuLoadingConfiguration restore;
        const auto permissive = GENERATE(false, true);
        ObjLoading::Configuration.MenuPermissiveParsing = permissive;
        ObjLoading::Configuration.MenuNoOptimization = GENERATE(false, true);

        Zone zone("MockZone", 0, GameId::IW5, GamePlatform::PC);
        MockSearchPath searchPath;
        searchPath.AddFileData("test.txt", R"MENU({ menuDef { name "test" itemDef { text "fallback" exp text (3); } } })MENU");
        const auto creator = ::menu::CreateMenuListLoaderIW5(zone.Memory(), searchPath);
        AssetCreatorCollection creators(zone);
        IgnoredAssetLookup ignored;
        AssetCreationContext context(zone, &creators, &ignored);

        REQUIRE(creator->CreateAsset("test.txt", context).HasBeenSuccessful() == permissive);
        if (!permissive)
            return;

        const auto* menuInfo = zone.m_pools.GetAsset(ASSET_TYPE_MENU, "test");
        REQUIRE(menuInfo);
        const auto* menu = static_cast<const menuDef_t*>(menuInfo->m_ptr);
        REQUIRE(menu->itemCount == 1);
        const auto* item = menu->items[0];
        REQUIRE(item->text);
        CHECK(std::string(item->text) == "fallback");
        const auto* expression = item->textExp;
        REQUIRE(expression);
        REQUIRE(expression->numEntries == 1);
        CHECK(expression->entries[0].type == EET_OPERAND);
        CHECK(expression->entries[0].data.operand.dataType == VAL_INT);
        CHECK(expression->entries[0].data.operand.internals.intVal == 3);
    }
} // namespace test::game::iw5::menu::parsing::it
