#include "Game/IW5/Menu/LoaderMenuListIW5.h"

#include "ObjLoading.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <string>

using namespace IW5;
using namespace Catch;

namespace
{
    struct RestoreMenuLoadingConfiguration
    {
        ObjLoading::Configuration_t original{ObjLoading::Configuration};

        ~RestoreMenuLoadingConfiguration()
        {
            ObjLoading::Configuration = original;
        }
    };

    class MenuLoadingTestHelper
    {
    public:
        MenuLoadingTestHelper()
            : m_zone("MockZone", 0, GameId::IW5, GamePlatform::PC),
              m_creator_collection(m_zone),
              m_context(m_zone, &m_creator_collection, &m_ignored_asset_lookup)
        {
            m_loader = menu::CreateMenuListLoaderIW5(m_zone.Memory(), m_search_path);
        }

        void AddFile(std::string fileName, std::string data)
        {
            m_search_path.AddFileData(std::move(fileName), std::move(data));
        }

        AssetCreationResult LoadMenuList(const std::string& name)
        {
            return m_loader->CreateAsset(name, m_context);
        }

        [[nodiscard]] menuDef_t* GetMenu(const std::string& name) const
        {
            const auto* asset = m_zone.m_pools.GetAsset(ASSET_TYPE_MENU, name);
            REQUIRE(asset);
            return static_cast<menuDef_t*>(asset->m_ptr);
        }

    private:
        Zone m_zone;
        MockSearchPath m_search_path;
        std::unique_ptr<IAssetCreator> m_loader;
        AssetCreatorCollection m_creator_collection;
        IgnoredAssetLookup m_ignored_asset_lookup;
        AssetCreationContext m_context;
    };
} // namespace

TEST_CASE("MenuParsingIW5IT: Integer text expressions respect permissive mode", "[parsing][converting][menu][it]")
{
    RestoreMenuLoadingConfiguration restore;
    const auto permissive = GENERATE(false, true);
    const auto optimization = GENERATE(false, true);
    CAPTURE(permissive, optimization);
    ObjLoading::Configuration.MenuPermissiveParsing = permissive;
    ObjLoading::Configuration.MenuNoOptimization = !optimization;

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

    if (optimization)
    {
        CHECK(std::string(item->text) == "3");
        REQUIRE(!item->textExp);
    }
    else
    {
        CHECK(std::string(item->text) == "fallback");
        const auto* expression = item->textExp;
        REQUIRE(expression);
        REQUIRE(expression->numEntries == 1);
        CHECK(expression->entries[0].type == EET_OPERAND);
        CHECK(expression->entries[0].data.operand.dataType == VAL_INT);
        CHECK(expression->entries[0].data.operand.internals.intVal == 3);
    }
}

TEST_CASE("MenuParsingIW5IT: Double text expressions respect permissive mode", "[parsing][converting][menu][it]")
{
    RestoreMenuLoadingConfiguration restore;
    const auto permissive = GENERATE(false, true);
    const auto optimization = GENERATE(false, true);
    CAPTURE(permissive, optimization);
    ObjLoading::Configuration.MenuPermissiveParsing = permissive;
    ObjLoading::Configuration.MenuNoOptimization = !optimization;

    Zone zone("MockZone", 0, GameId::IW5, GamePlatform::PC);
    MockSearchPath searchPath;
    searchPath.AddFileData("test.txt", R"MENU({ menuDef { name "test" itemDef { text "fallback" exp text (3.5); } } })MENU");
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

    if (optimization)
    {
        CHECK(std::string(item->text) == "3.5");
        REQUIRE(!item->textExp);
    }
    else
    {
        CHECK(std::string(item->text) == "fallback");
        const auto* expression = item->textExp;
        REQUIRE(expression);
        REQUIRE(expression->numEntries == 1);
        CHECK(expression->entries[0].type == EET_OPERAND);
        CHECK(expression->entries[0].data.operand.dataType == VAL_FLOAT);
        CHECK(expression->entries[0].data.operand.internals.floatVal == Approx(3.5));
    }
}

TEST_CASE("MenuParsingIW5IT: Keeps the menus of a menu list loaded by an earlier one", "[parsing][converting][menu][it]")
{
    MenuLoadingTestHelper helper;

    helper.AddFile("ui_mp/ingame.txt", R"testmenu(
{
    loadMenu { "ui_mp/scriptmenus/killstreak.menu" }
}
)testmenu");

    helper.AddFile("ui_mp/scriptmenus/killstreak.menu", R"testmenu(
{
    loadMenu { "ui_mp/killstreak_hint.menu" }
    menuDef
    {
        name "killstreak"
    }
}
)testmenu");

    helper.AddFile("ui_mp/killstreak_hint.menu", R"testmenu(
{
    menuDef
    {
        name "killstreak_hint"
    }
}
)testmenu");

    REQUIRE(helper.LoadMenuList("ui_mp/ingame.txt").HasBeenSuccessful());

    const auto result = helper.LoadMenuList("ui_mp/scriptmenus/killstreak.menu");
    REQUIRE(result.HasBeenSuccessful());

    const auto* menuList = static_cast<const MenuList*>(result.GetAssetInfo()->m_ptr);
    REQUIRE(menuList->menuCount == 2);
    REQUIRE(menuList->menus[0] == helper.GetMenu("killstreak"));
    REQUIRE(menuList->menus[1] == helper.GetMenu("killstreak_hint"));
}
