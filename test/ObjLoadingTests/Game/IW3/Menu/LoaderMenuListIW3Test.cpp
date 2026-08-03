#include "Game/IW3/Menu/LoaderMenuListIW3.h"

#include "Game/IW3/GameIW3.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <memory>
#include <string>

using namespace IW3;
using namespace std::literals;
using namespace Catch::Matchers;

namespace
{
    class MenuLoadingTestHelper
    {
    public:
        MenuLoadingTestHelper()
            : m_zone("MockZone", 0, GameId::IW3, GamePlatform::PC),
              m_creator_collection(m_zone),
              m_context(m_zone, &m_creator_collection, &m_ignored_asset_lookup)
        {
            m_loader = menu::CreateMenuListLoaderIW3(m_zone.Memory(), m_search_path);
        }

        void AddFile(std::string name, std::string contents)
        {
            m_search_path.AddFileData(std::move(name), std::move(contents));
        }

        Material* AddMaterial(const std::string& name)
        {
            auto* material = m_zone.Memory().Alloc<Material>();
            material->info.name = m_zone.Memory().Dup(name.c_str());
            m_context.AddAsset<AssetMaterial>(name, material);
            return material;
        }

        AssetCreationResult LoadMenuList(const std::string& name)
        {
            return m_loader->CreateAsset(name, m_context);
        }

        menuDef_t* GetMenu(const std::string& name)
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

    TEST_CASE("MenuListLoaderIW3: Loads dumped menus and preserves IW3 data", "[iw3][menu][assetloader]")
    {
        MenuLoadingTestHelper helper;
        const auto* backgroundMaterial = helper.AddMaterial("background_material");
        const auto* selectMaterial = helper.AddMaterial("select_material");

        helper.AddFile("ui_mp/menus.txt", R"MENU({
    loadMenu { "ui_mp/main.menu" }
    loadMenu { "ui_mp/popup.menu" }
}
)MENU");
        helper.AddFile("ui_mp/main.menu", R"MENU({
    menuDef
    {
        name "main"
        fullscreen 1
        rect 0 0 640 480 0 0
        background "background_material"
        forecolor 0.8 0.7 0.6 0.5
        disablecolor 0.2 0.3 0.4 0.5
        visible when(dvarbool("ui_show_main"));
        onOpen
        {
            play menu_open;
            setLocalVarInt ui_highlight 5;
        }
        itemDef
        {
            name "empty_text"
            text ""
            rect 10 20 180 24 1 2
            type 1
            visible 1
            forecolor 0.9 0.9 1 0.07
            dvar "ui_fallback_text"
            action
            {
                setItemColor empty_text borderColor 0.1 0.1 0.12 0.5;
                execondvarstringvalue ui_zfeather 0 "set cg_laserLight 0";
            }
            exp material (localvarstring("ui_material"));
        }
        itemDef
        {
            name "server_list"
            rect 20 60 300 200 0 0
            type 6
            columns 2 0 120 32 0 120 180 48 1
            elementwidth 300
            elementheight 20
            feeder 2
            elementtype 1
            notselectable
            noscrollbars
            usepaging
            disablecolor 0.6 0.5 0.4 0.3
            selectBorder 1 1 1 1
            selectIcon "select_material"
            doubleclick
            {
                play mouse_click;
            }
        }
        itemDef
        {
            name "default_text"
            text "-"
            maxChars 2
        }
    }
}
)MENU");
        helper.AddFile("ui_mp/popup.menu", R"MENU({
    menuDef
    {
        name "popup"
        rect 40 40 200 100 0 0
        popup
        onESC
        {
            close self;
        }
    }
}
)MENU");

        const auto result = helper.LoadMenuList("ui_mp/menus.txt");
        REQUIRE(result.HasBeenSuccessful());

        const auto* menuList = static_cast<MenuList*>(result.GetAssetInfo()->m_ptr);
        const auto* mainMenu = helper.GetMenu("main");
        const auto* popupMenu = helper.GetMenu("popup");

        REQUIRE(menuList->name == "ui_mp/menus.txt"s);
        REQUIRE(menuList->menuCount == 2);
        REQUIRE(menuList->menus[0] == mainMenu);
        REQUIRE(menuList->menus[1] == popupMenu);

        REQUIRE(mainMenu->window.background == backgroundMaterial);
        REQUIRE(mainMenu->window.dynamicFlags[0] & 0x10000);
        REQUIRE_THAT(mainMenu->disableColor[0], WithinRel(0.2f));
        REQUIRE_THAT(mainMenu->disableColor[3], WithinRel(0.5f));
        REQUIRE(mainMenu->visibleExp.numEntries == 3);
        REQUIRE(mainMenu->visibleExp.entries[0]->type == EET_OPERATOR);
        REQUIRE(mainMenu->visibleExp.entries[0]->data.op == OP_DVARBOOL);
        REQUIRE(mainMenu->visibleExp.entries[1]->data.operand.dataType == VAL_STRING);
        REQUIRE(mainMenu->visibleExp.entries[1]->data.operand.internals.stringVal == "ui_show_main"s);
        REQUIRE(mainMenu->visibleExp.entries[2]->data.op == OP_RIGHTPAREN);
        REQUIRE(mainMenu->onOpen == R"("play" "menu_open" ; "setLocalVarInt" "ui_highlight" "5" ; )"s);

        REQUIRE(mainMenu->itemCount == 3);
        const auto* emptyTextItem = mainMenu->items[0];
        REQUIRE(emptyTextItem->parent == mainMenu);
        REQUIRE(emptyTextItem->text != nullptr);
        REQUIRE(emptyTextItem->text == ""s);
        REQUIRE(emptyTextItem->dvar == "ui_fallback_text"s);
        REQUIRE(emptyTextItem->window.dynamicFlags[0] & 0x4);
        REQUIRE(emptyTextItem->window.dynamicFlags[0] & 0x10000);
        REQUIRE(
            emptyTextItem->action
            == R"("setItemColor" "empty_text" "borderColor" "0.1" "0.1" "0.12" "0.5" ; "execondvarstringvalue" "ui_zfeather" "0" "set cg_laserLight 0" ; )"s);
        REQUIRE(emptyTextItem->materialExp.numEntries == 3);
        REQUIRE(emptyTextItem->materialExp.entries[0]->data.op == OP_LOCALVARSTRING);

        const auto* listItem = mainMenu->items[1];
        REQUIRE(listItem->typeData.listBox);
        REQUIRE(listItem->typeData.listBox->numColumns == 2);
        REQUIRE(listItem->typeData.listBox->columnInfo[1].pos == 120);
        REQUIRE(listItem->typeData.listBox->columnInfo[1].width == 180);
        REQUIRE(listItem->typeData.listBox->notselectable == 1);
        REQUIRE(listItem->typeData.listBox->noScrollBars == 1);
        REQUIRE(listItem->typeData.listBox->usePaging == 1);
        REQUIRE_THAT(listItem->typeData.listBox->disableColor[0], WithinRel(0.6f));
        REQUIRE(listItem->typeData.listBox->selectIcon == selectMaterial);
        REQUIRE(listItem->typeData.listBox->doubleClick == R"("play" "mouse_click" ; )"s);

        const auto* defaultTextItem = mainMenu->items[2];
        REQUIRE(defaultTextItem->type == 0);
        REQUIRE(defaultTextItem->typeData.editField);
        REQUIRE(defaultTextItem->typeData.editField->maxChars == 2);

        REQUIRE(popupMenu->window.staticFlags & 0x01000000);
        REQUIRE(popupMenu->onESC == R"("close" "self" ; )"s);
    }
} // namespace
