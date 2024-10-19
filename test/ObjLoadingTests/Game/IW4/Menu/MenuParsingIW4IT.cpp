#include "Game/IW4/AssetLoaders/AssetLoaderMenuList.h"
#include "Game/IW4/GameIW4.h"
#include "Mock/MockAssetLoadingManager.h"
#include "Mock/MockSearchPath.h"
#include "Parsing/Menu/MenuFileReader.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <string>

using namespace menu;
using namespace IW4;
using namespace std::literals;
using namespace Catch::Matchers;

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
            : m_zone("MockZone", 0, IGame::GetGameById(GameId::IW4)),
              m_manager(m_zone, m_search_path)
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

        Material* AddMaterial(const std::string& name)
        {
            auto* material = m_zone.GetMemory()->Create<Material>();
            material->info.name = m_zone.GetMemory()->Dup(name.c_str());

            m_manager.MockAddAvailableDependency(ASSET_TYPE_MATERIAL, name, material);

            return material;
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
#define KEY_LEET 1337
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
		onRequestClose
		{
			play "nope";
		}
		onESC
		{
			setColor backColor 1 0.5 0.1 1;
		}
		border 1
		borderSize 1.25
		backColor 1 0.8 0.4 0.95
		foreColor 0.7
		background	"funny_dog.png"
		execKey "q"
		{
			exec "quit"
		}
		execKeyInt KEY_LEET
		{
			exec "vid_restart"
		}
	}
}
			)testmenu");

        const auto* funnyDogMaterial = helper.AddMaterial("funny_dog.png");

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
        REQUIRE_THAT(menu->window.rect.x, WithinRel(420.0f));
        REQUIRE_THAT(menu->window.rect.y, WithinRel(1337.0f));
        REQUIRE_THAT(menu->window.rect.w, WithinRel(12.0f));
        REQUIRE_THAT(menu->window.rect.h, WithinRel(64.0f));
        REQUIRE(menu->window.rect.horzAlign == 1);
        REQUIRE(menu->window.rect.vertAlign == 2);
        REQUIRE(menu->window.style == 5);

        REQUIRE(menu->visibleExp == nullptr);
        REQUIRE(menu->window.dynamicFlags[0] & WINDOW_FLAG_VISIBLE);

        REQUIRE(menu->onOpen != nullptr);
        REQUIRE(menu->onOpen->eventHandlerCount == 1);
        REQUIRE(menu->onOpen->eventHandlers[0]->eventType == EventType::EVENT_UNCONDITIONAL);
        REQUIRE(menu->onOpen->eventHandlers[0]->eventData.unconditionalScript != nullptr);
        REQUIRE(menu->onOpen->eventHandlers[0]->eventData.unconditionalScript
                == R"("focusFirst" ; "play" "fart_sound" ; "exec" "wait 1; set r_fullbright 1" ; )"s);

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
        REQUIRE(menu->onClose->eventHandlers[1]->eventData.conditionalScript->eventHandlerSet->eventHandlers[0]->eventData.unconditionalScript
                == R"("exec" "quit" ; )"s);
        REQUIRE(menu->onClose->eventHandlers[2]->eventType == EventType::EVENT_ELSE);
        REQUIRE(menu->onClose->eventHandlers[2]->eventData.elseScript != nullptr);
        REQUIRE(menu->onClose->eventHandlers[2]->eventData.elseScript->eventHandlerCount == 1);
        REQUIRE(menu->onClose->eventHandlers[2]->eventData.elseScript->eventHandlers != nullptr);
        REQUIRE(menu->onClose->eventHandlers[2]->eventData.elseScript->eventHandlers[0] != nullptr);
        REQUIRE(menu->onClose->eventHandlers[2]->eventData.elseScript->eventHandlers[0]->eventType == EventType::EVENT_UNCONDITIONAL);
        REQUIRE(menu->onClose->eventHandlers[2]->eventData.elseScript->eventHandlers[0]->eventData.unconditionalScript
                == R"("uiScript" "startSingleplayer" ; )"s);
        REQUIRE(menu->onClose->eventHandlers[3]->eventType == EventType::EVENT_UNCONDITIONAL);
        REQUIRE(menu->onClose->eventHandlers[3]->eventData.unconditionalScript != nullptr);
        REQUIRE(menu->onClose->eventHandlers[3]->eventData.unconditionalScript == R"("setGameMode" "dm" ; )"s);

        REQUIRE(menu->onCloseRequest != nullptr);
        REQUIRE(menu->onCloseRequest->eventHandlerCount == 1);
        REQUIRE(menu->onCloseRequest->eventHandlers[0]->eventType == EventType::EVENT_UNCONDITIONAL);
        REQUIRE(menu->onCloseRequest->eventHandlers[0]->eventData.unconditionalScript != nullptr);
        REQUIRE(menu->onCloseRequest->eventHandlers[0]->eventData.unconditionalScript == R"("play" "nope" ; )"s);

        REQUIRE(menu->onESC != nullptr);
        REQUIRE(menu->onESC->eventHandlerCount == 1);
        REQUIRE(menu->onESC->eventHandlers[0]->eventType == EventType::EVENT_UNCONDITIONAL);
        REQUIRE(menu->onESC->eventHandlers[0]->eventData.unconditionalScript != nullptr);
        REQUIRE(menu->onESC->eventHandlers[0]->eventData.unconditionalScript == R"("setColor" "backColor" "1" "0.5" "0.1" "1" ; )"s);

        REQUIRE(menu->window.border == 1);
        REQUIRE_THAT(menu->window.borderSize, WithinRel(1.25f));
        REQUIRE_THAT(menu->window.borderSize, WithinRel(1.25));
        REQUIRE_THAT(menu->window.backColor[0], WithinRel(1.0f));
        REQUIRE_THAT(menu->window.backColor[1], WithinRel(0.8f));
        REQUIRE_THAT(menu->window.backColor[2], WithinRel(0.4f));
        REQUIRE_THAT(menu->window.backColor[3], WithinRel(0.95f));
        REQUIRE_THAT(menu->window.foreColor[0], WithinRel(0.7f));
        REQUIRE_THAT(menu->window.foreColor[1], WithinRel(0.0f));
        REQUIRE_THAT(menu->window.foreColor[2], WithinRel(0.0f));
        REQUIRE_THAT(menu->window.foreColor[3], WithinRel(0.0f));
        REQUIRE(menu->window.background == funnyDogMaterial);

        REQUIRE(menu->onKey != nullptr);
        REQUIRE(menu->onKey->next != nullptr);
        REQUIRE(menu->onKey->next->next == nullptr);

        const auto keyHandler1 = menu->onKey;
        const auto keyHandler2 = menu->onKey->next;

        ItemKeyHandler* qKeyHandler;
        ItemKeyHandler* leetKeyHandler;
        if (keyHandler1->key == 'q')
        {
            qKeyHandler = keyHandler1;
            leetKeyHandler = keyHandler2;
        }
        else
        {
            leetKeyHandler = keyHandler1;
            qKeyHandler = keyHandler2;
        }

        REQUIRE(qKeyHandler->key == 'q');
        REQUIRE(qKeyHandler->action->eventHandlerCount == 1);
        REQUIRE(qKeyHandler->action->eventHandlers != nullptr);
        REQUIRE(qKeyHandler->action->eventHandlers[0] != nullptr);
        REQUIRE(qKeyHandler->action->eventHandlers[0]->eventType == EVENT_UNCONDITIONAL);
        REQUIRE(qKeyHandler->action->eventHandlers[0]->eventData.unconditionalScript == R"("exec" "quit" ; )"s);

        REQUIRE(leetKeyHandler->key == 1337);
        REQUIRE(leetKeyHandler->action->eventHandlerCount == 1);
        REQUIRE(leetKeyHandler->action->eventHandlers != nullptr);
        REQUIRE(leetKeyHandler->action->eventHandlers[0] != nullptr);
        REQUIRE(leetKeyHandler->action->eventHandlers[0]->eventType == EVENT_UNCONDITIONAL);
        REQUIRE(leetKeyHandler->action->eventHandlers[0]->eventData.unconditionalScript == R"("exec" "vid_restart" ; )"s);

        REQUIRE(menu->itemCount == 0);
        REQUIRE(menu->items == nullptr);
    }

    TEST_CASE("MenuParsingIW4IT: Can specify event handler multiple times", "[parsing][converting][menu][it]")
    {
        MenuParsingItHelper helper;

        helper.AddFile(R"testmenu(
{
	menuDef
	{
		name  "Blab"
		onOpen
		{
			focusFirst;
		}
		onOpen
		{
			play "fart_sound";
		}
		onOpen
		{
			exec "wait 1; set r_fullbright 1";
		}

        itemDef
        {
            action
            {
                play "lmfao";
            }
            action
            {
                play "lol";
            }
        }
	}
}
			)testmenu");

        const auto result = helper.RunIntegrationTest();
        REQUIRE(result);

        const auto* menuList = helper.GetMenuListAsset();
        const auto* menu = helper.GetMenuAsset("Blab");

        REQUIRE(menuList->menuCount == 1);
        REQUIRE(menuList->menus);

        REQUIRE(menuList->menus[0] == menu);

        REQUIRE(menu->window.name == "Blab"s);

        REQUIRE(menu->onOpen != nullptr);
        REQUIRE(menu->onOpen->eventHandlerCount == 3);
        REQUIRE(menu->onOpen->eventHandlers[0]->eventType == EventType::EVENT_UNCONDITIONAL);
        REQUIRE(menu->onOpen->eventHandlers[0]->eventData.unconditionalScript != nullptr);
        REQUIRE(menu->onOpen->eventHandlers[0]->eventData.unconditionalScript == R"("focusFirst" ; )"s);
        REQUIRE(menu->onOpen->eventHandlers[1]->eventType == EventType::EVENT_UNCONDITIONAL);
        REQUIRE(menu->onOpen->eventHandlers[1]->eventData.unconditionalScript != nullptr);
        REQUIRE(menu->onOpen->eventHandlers[1]->eventData.unconditionalScript == R"("play" "fart_sound" ; )"s);
        REQUIRE(menu->onOpen->eventHandlers[2]->eventType == EventType::EVENT_UNCONDITIONAL);
        REQUIRE(menu->onOpen->eventHandlers[2]->eventData.unconditionalScript != nullptr);
        REQUIRE(menu->onOpen->eventHandlers[2]->eventData.unconditionalScript == R"("exec" "wait 1; set r_fullbright 1" ; )"s);

        REQUIRE(menu->itemCount == 1);
        REQUIRE(menu->items != nullptr);

        const auto* item = menu->items[0];
        REQUIRE(item != nullptr);

        REQUIRE(item->action != nullptr);
        REQUIRE(item->action->eventHandlerCount == 2);
        REQUIRE(item->action->eventHandlers[0]->eventType == EventType::EVENT_UNCONDITIONAL);
        REQUIRE(item->action->eventHandlers[0]->eventData.unconditionalScript != nullptr);
        REQUIRE(item->action->eventHandlers[0]->eventData.unconditionalScript == R"("play" "lmfao" ; )"s);
        REQUIRE(item->action->eventHandlers[1]->eventType == EventType::EVENT_UNCONDITIONAL);
        REQUIRE(item->action->eventHandlers[1]->eventData.unconditionalScript != nullptr);
        REQUIRE(item->action->eventHandlers[1]->eventData.unconditionalScript == R"("play" "lol" ; )"s);
    }
} // namespace test::game::iw4::menu::parsing::it
