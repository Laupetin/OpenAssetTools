#include <catch2/catch.hpp>

#include <string>

#include "Game/IW4/GameIW4.h"
#include "Game/IW4/AssetLoaders/AssetLoaderMenuList.h"
#include "Mock/MockAssetLoadingManager.h"
#include "Mock/MockSearchPath.h"
#include "Parsing/Menu/MenuFileReader.h"
#include "Utils/MemoryManager.h"

using namespace menu;
using namespace IW4;
using namespace std::literals;

#define L(x) x "\n"

namespace test::game::iw4::menu::parsing
{
	class MenuParsingItHelper
	{
		static constexpr const char* DEFAULT_ASSET_NAME = "test.txt";

		Zone m_zone;
		MockSearchPath m_search_path;
		MockAssetLoadingManager m_manager;
		AssetLoaderMenuList m_asset_loader;

	public:
		MenuParsingItHelper()
			: m_zone("MockZone", 0, &g_GameIW4),
			  m_manager(&m_zone, &m_search_path)
		{
		}

		void AddFile(std::string fileName, std::string data)
		{
			m_search_path.AddFileData(std::move(fileName), std::move(data));
		}

		void AddFile(std::string data)
		{
			AddFile(DEFAULT_ASSET_NAME, std::move(data));
		}

		bool RunIntegrationTest()
		{
			return m_asset_loader.LoadFromRaw(DEFAULT_ASSET_NAME, &m_search_path, m_zone.GetMemory(), &m_manager, &m_zone);
		}

		MenuList* GetMenuListAsset()
		{
			const auto addedAsset = m_manager.MockGetAddedAsset(DEFAULT_ASSET_NAME);
			REQUIRE(addedAsset);
			REQUIRE(addedAsset->m_type == ASSET_TYPE_MENULIST);

			return static_cast<MenuList*>(addedAsset->m_ptr);
		}

		menuDef_t* GetMenuAsset(const std::string& menuName)
		{
			const auto addedAsset = m_manager.MockGetAddedAsset(menuName);
			REQUIRE(addedAsset);
			REQUIRE(addedAsset->m_type == ASSET_TYPE_MENU);

			return static_cast<menuDef_t*>(addedAsset->m_ptr);
		}
	};

	TEST_CASE("MenuParsingIW4IT: Can convert simple menu without properties", "[parsing][converting][menu][it]")
	{
		MenuParsingItHelper helper;

		helper.AddFile(""
			L("{")
			L("  menuDef")
			L("  {")
			L("    name  \"Hello\"")
			L("  }")
			L("}")
		);

		const auto result = helper.RunIntegrationTest();
		REQUIRE(result);

		const auto* menuList = helper.GetMenuListAsset();
		const auto* menu = helper.GetMenuAsset("Hello");

		REQUIRE(menuList->menuCount == 1);
		REQUIRE(menuList->menus);

		REQUIRE(menuList->menus[0] == menu);

		REQUIRE(menu->window.name == "Hello"s);
		REQUIRE(menu->itemCount == 0);
		REQUIRE(menu->items == nullptr);
	}

	TEST_CASE("MenuParsingIW4IT: Can convert simple menu", "[parsing][converting][menu][it]")
	{
		MenuParsingItHelper helper;

		helper.AddFile(""
			L("#define COOL_STYLE 5")
			L("")
			L("{")
			L("  menuDef")
			L("  {")
			L("    name  \"Bla\"")
			L("    fullScreen 1")
			L("    screenSpace")
			L("    decoration")
			L("    rect (351 + 69) (1336 + 1) 12 64 1 2")
			L("    style COOL_STYLE")
			L("  }")
			L("}")
		);

		const auto result = helper.RunIntegrationTest();
		REQUIRE(result);

		const auto* menuList = helper.GetMenuListAsset();
		const auto* menu = helper.GetMenuAsset("Bla");

		REQUIRE(menuList->menuCount == 1);
		REQUIRE(menuList->menus);

		REQUIRE(menuList->menus[0] == menu);

		REQUIRE(menu->window.name == "Bla"s);
		REQUIRE(menu->fullScreen > 0);
		REQUIRE(menu->window.staticFlags & WINDOW_FLAG_SCREEN_SPACE);
		REQUIRE(menu->window.staticFlags & WINDOW_FLAG_DECORATION);
		REQUIRE(menu->window.rect.x == Approx(420));
		REQUIRE(menu->window.rect.y == Approx(1337));
		REQUIRE(menu->window.rect.w == Approx(12));
		REQUIRE(menu->window.rect.h == Approx(64));
		REQUIRE(menu->window.rect.horzAlign == 1);
		REQUIRE(menu->window.rect.vertAlign == 2);
		REQUIRE(menu->window.style == 5);
		REQUIRE(menu->itemCount == 0);
		REQUIRE(menu->items == nullptr);
	}
}
