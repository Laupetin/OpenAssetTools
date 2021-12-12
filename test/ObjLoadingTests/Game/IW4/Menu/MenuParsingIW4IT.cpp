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

namespace test::game::iw4::menu::parsing::it
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

		helper.AddFile(R"testmenu(
{
	menuDef
	{
		name "Hello"
	}
}
			)testmenu");

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

		helper.AddFile(R"testmenu(
#define COOL_STYLE 5
#define VISIBILITY_ENABLED 1
#define HIDE_STUPID_ELEMENTS 0
{
	menuDef
	{
		name  "Bla"
		fullScreen 1
		screenSpace
		decoration
		rect (351 + 69) (1336 + 1) 12 64 1 2
		style COOL_STYLE
		visible when( VISIBILITY_ENABLED && !HIDE_STUPID_ELEMENTS )
		onOpen
		{
			focusFirst;
			play "fart_sound";
			exec "wait 1; set r_fullbright 1";
		}
		onClose
		{
			setBackground "cute_cat.jpg";
			if( anynewmappacks() && isreloading() )
			{
				exec "quit";
			}
			else
			{
				uiScript startSingleplayer;
			}
			setGameMode "dm"
		}
	}
}
			)testmenu");

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

		REQUIRE(menu->visibleExp != nullptr);
		REQUIRE(menu->visibleExp->entries != nullptr);
		REQUIRE(menu->visibleExp->numEntries == 1);
		REQUIRE(menu->visibleExp->entries[0].type == expressionEntryType::EET_OPERAND);
		REQUIRE(menu->visibleExp->entries[0].data.operand.dataType == expDataType::VAL_INT);
		REQUIRE(menu->visibleExp->entries[0].data.operand.internals.intVal > 0);

		REQUIRE(menu->onOpen != nullptr);
		REQUIRE(menu->onOpen->eventHandlerCount == 1);
		REQUIRE(menu->onOpen->eventHandlers[0]->eventType == EventType::EVENT_UNCONDITIONAL);
		REQUIRE(menu->onOpen->eventHandlers[0]->eventData.unconditionalScript != nullptr);
		REQUIRE(menu->onOpen->eventHandlers[0]->eventData.unconditionalScript == R"("focusFirst" ; "play" "fart_sound" ; "exec" "wait 1; set r_fullbright 1" ; )"s);

		REQUIRE(menu->onClose != nullptr);
		REQUIRE(menu->onClose->eventHandlerCount == 4);
		REQUIRE(menu->onClose->eventHandlers[0]->eventType == EventType::EVENT_UNCONDITIONAL);
		REQUIRE(menu->onClose->eventHandlers[0]->eventData.unconditionalScript != nullptr);
		REQUIRE(menu->onClose->eventHandlers[0]->eventData.unconditionalScript == R"("setBackground" "cute_cat.jpg" ; )"s);
		REQUIRE(menu->onClose->eventHandlers[1]->eventType == EventType::EVENT_IF);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript != nullptr);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventExpression != nullptr);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventExpression->numEntries == 5);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventExpression->entries[0].type == expressionEntryType::EET_OPERATOR);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventExpression->entries[0].data.op == 102); // anynewmappacks
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventExpression->entries[1].type == expressionEntryType::EET_OPERATOR);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventExpression->entries[1].data.op == OP_RIGHTPAREN);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventExpression->entries[2].type == expressionEntryType::EET_OPERATOR);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventExpression->entries[2].data.op == OP_AND);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventExpression->entries[3].type == expressionEntryType::EET_OPERATOR);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventExpression->entries[3].data.op == 118); // isreloading
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventExpression->entries[4].type == expressionEntryType::EET_OPERATOR);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventExpression->entries[4].data.op == OP_RIGHTPAREN);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventHandlerSet != nullptr);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventHandlerSet->eventHandlerCount == 1);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventHandlerSet->eventHandlers != nullptr);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventHandlerSet->eventHandlers[0] != nullptr);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventHandlerSet->eventHandlers[0]->eventType == EventType::EVENT_UNCONDITIONAL);
		REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventHandlerSet->eventHandlers[0]->eventData.unconditionalScript == R"("exec" "quit" ; )"s);
		REQUIRE(menu->onClose->eventHandlers[2]->eventType == EventType::EVENT_ELSE);
		REQUIRE(menu->onClose->eventHandlers[2]->eventData.elseScript != nullptr);
		REQUIRE(menu->onClose->eventHandlers[2]->eventData.elseScript->eventHandlerCount == 1);
		REQUIRE(menu->onClose->eventHandlers[2]->eventData.elseScript->eventHandlers != nullptr);
		REQUIRE(menu->onClose->eventHandlers[2]->eventData.elseScript->eventHandlers[0] != nullptr);
		REQUIRE(menu->onClose->eventHandlers[2]->eventData.elseScript->eventHandlers[0]->eventType == EventType::EVENT_UNCONDITIONAL);
		REQUIRE(menu->onClose->eventHandlers[2]->eventData.elseScript->eventHandlers[0]->eventData.unconditionalScript == R"("uiScript" "startSingleplayer" ; )"s);
		REQUIRE(menu->onClose->eventHandlers[3]->eventType == EventType::EVENT_UNCONDITIONAL);
		REQUIRE(menu->onClose->eventHandlers[3]->eventData.unconditionalScript != nullptr);
		REQUIRE(menu->onClose->eventHandlers[3]->eventData.unconditionalScript == R"("setGameMode" "dm" ; )"s);

		REQUIRE(menu->itemCount == 0);
		REQUIRE(menu->items == nullptr);
	}
}
