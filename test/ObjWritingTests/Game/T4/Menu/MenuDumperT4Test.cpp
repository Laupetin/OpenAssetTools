#include "Game/T4/Menu/MenuDumperT4.h"

#include "Game/T4/Menu/MenuListDumperT4.h"
#include "Parsing/Menu/MenuFileReader.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <memory>
#include <sstream>
#include <string>

using namespace T4;

namespace
{
    expressionEntry Operator(const operationEnum operation)
    {
        expressionEntry entry{};
        entry.type = EET_OPERATOR;
        entry.data.op = operation;
        return entry;
    }

    expressionEntry IntOperand(const int value)
    {
        expressionEntry entry{};
        entry.type = EET_OPERAND;
        entry.data.operand.dataType = VAL_INT;
        entry.data.operand.internals.intVal = value;
        return entry;
    }

    expressionEntry StringOperand(const char* value)
    {
        expressionEntry entry{};
        entry.type = EET_OPERAND;
        entry.data.operand.dataType = VAL_STRING;
        entry.data.operand.internals.string = value;
        return entry;
    }

    template<std::size_t EntryCount> std::array<expressionEntry*, EntryCount> EntryPointers(std::array<expressionEntry, EntryCount>& entries)
    {
        std::array<expressionEntry*, EntryCount> result{};
        for (auto entryIndex = 0u; entryIndex < EntryCount; entryIndex++)
            result[entryIndex] = &entries[entryIndex];
        return result;
    }

    TEST_CASE("MenuDumperT4: Can dump menu with expressions and scripts", "[t4][menu][assetdumper]")
    {
        constexpr auto UI_SHOW_FAVORITE_SERVERS = 0x00000004;
        constexpr auto UI_SHOW_NOT_FAVORITE_SERVERS = 0x00001000;

        std::array menuVisibleValues{
            Operator(OP_DVARBOOL),
            StringOperand("ui_test"),
            Operator(OP_RIGHTPAREN),
        };
        auto menuVisibleEntries = EntryPointers(menuVisibleValues);

        std::array buttonVisibleValues{
            Operator(OP_LEFTPAREN),
            Operator(OP_LOCALVARINT),
            StringOperand("ui_highlight"),
            Operator(OP_RIGHTPAREN),
            Operator(OP_EQUALS),
            IntOperand(5),
            Operator(OP_AND),
            Operator(OP_LOCALVARSTRING),
            StringOperand("ui_choicegroup"),
            Operator(OP_RIGHTPAREN),
            Operator(OP_EQUALS),
            StringOperand("popmenu"),
            Operator(OP_RIGHTPAREN),
        };
        auto buttonVisibleEntries = EntryPointers(buttonVisibleValues);

        std::array materialExpressionValues{
            Operator(OP_LEFTPAREN),
            StringOperand("expression_material"),
            Operator(OP_RIGHTPAREN),
        };
        auto materialExpressionEntries = EntryPointers(materialExpressionValues);

        Material backgroundMaterial{};
        backgroundMaterial.info.name = "background_material";

        ItemKeyHandler buttonKeyHandler{
            .key = 'a',
            .action = "open advanced;",
            .next = nullptr,
        };

        itemDef_s button{};
        button.window.name = "button_test";
        button.window.group = "buttons";
        button.window.rectClient = {
            .x = 10.0f,
            .y = 20.0f,
            .w = 180.0f,
            .h = 24.0f,
            .horzAlign = 1,
            .vertAlign = 2,
        };
        button.window.style = 1;
        button.window.ownerDrawFlags = UI_SHOW_NOT_FAVORITE_SERVERS;
        button.window.backColor[0] = 0.1f;
        button.window.backColor[1] = 0.1f;
        button.window.backColor[2] = 0.1f;
        button.window.backColor[3] = 0.25f;
        button.window.foreColor[0] = 0.69f;
        button.window.foreColor[1] = 0.69f;
        button.window.foreColor[2] = 0.69f;
        button.window.foreColor[3] = 1.0f;
        button.window.background = &backgroundMaterial;
        button.type = ITEM_TYPE_BUTTON;
        button.text = "@MENU_TEST";
        button.textAlignMode = 10;
        button.textalignx = -6.0f;
        button.textscale = 0.4f;
        button.textStyle = 6;
        button.fontEnum = 1;
        button.onFocus = "play mouse_over; setlocalvarint ui_highlight 5;";
        button.leaveFocus = "setlocalvarint ui_highlight 0;";
        button.mouseExit = R"(setitemcolor button_test bordercolor "0.1" "0.1" "0.12" "0.5";)";
        button.action = "play mouse_click; open options;";
        button.onKey = &buttonKeyHandler;
        button.visibleExp = {.numEntries = static_cast<int>(buttonVisibleEntries.size()), .entries = buttonVisibleEntries.data()};
        button.materialExp = {.numEntries = static_cast<int>(materialExpressionEntries.size()), .entries = materialExpressionEntries.data()};

        editFieldDef_s editField{};
        editField.minVal = -1.0f;
        editField.maxVal = -1.0f;
        editField.defVal = -1.0f;
        editField.maxChars = 16;
        editField.maxPaintChars = 16;

        itemDef_s editItem{};
        editItem.window.name = "name_field";
        editItem.window.group = "fields";
        editItem.window.rectClient = {.x = 10.0f, .y = 56.0f, .w = 180.0f, .h = 24.0f, .horzAlign = 1, .vertAlign = 2};
        editItem.window.style = 1;
        editItem.window.border = 1;
        editItem.window.borderSize = 1.0f;
        editItem.window.dynamicFlags[0] = WINDOW_FLAG_VISIBLE;
        editItem.window.backColor[0] = 0.1f;
        editItem.window.backColor[1] = 0.1f;
        editItem.window.backColor[2] = 0.1f;
        editItem.window.backColor[3] = 0.25f;
        editItem.window.foreColor[0] = 1.0f;
        editItem.window.foreColor[1] = 1.0f;
        editItem.window.foreColor[2] = 1.0f;
        editItem.window.foreColor[3] = 1.0f;
        editItem.window.borderColor[0] = 0.5f;
        editItem.window.borderColor[1] = 0.5f;
        editItem.window.borderColor[2] = 0.5f;
        editItem.window.borderColor[3] = 1.0f;
        editItem.type = ITEM_TYPE_UPREDITFIELD;
        editItem.text = "";
        editItem.textAlignMode = 1;
        editItem.textalignx = -28.0f;
        editItem.textaligny = 14.0f;
        editItem.textscale = 0.3f;
        editItem.fontEnum = 1;
        editItem.onAccept = "setdvar ui_name player_name;";
        editItem.dvar = "player_name";
        editItem.typeData.editField = &editField;

        itemDef_s* items[]{&button, &editItem};
        ItemKeyHandler menuKeyHandler{
            .key = 13,
            .action = "open key_help;",
            .next = nullptr,
        };

        menuDef_t menu{};
        menu.window.name = "test_menu";
        menu.window.rect = {.x = 0.0f, .y = 0.0f, .w = 640.0f, .h = 480.0f, .horzAlign = 0, .vertAlign = 0};
        menu.window.style = 1;
        menu.window.ownerDrawFlags = UI_SHOW_FAVORITE_SERVERS;
        menu.window.border = 1;
        menu.window.borderSize = 2.0f;
        menu.window.backColor[0] = 0.1f;
        menu.window.backColor[1] = 0.2f;
        menu.window.backColor[2] = 0.3f;
        menu.window.backColor[3] = 0.75f;
        menu.window.foreColor[0] = 1.0f;
        menu.window.foreColor[1] = 1.0f;
        menu.window.foreColor[2] = 1.0f;
        menu.window.foreColor[3] = 1.0f;
        menu.fullScreen = 1;
        menu.fadeCycle = 16;
        menu.fadeClamp = 1.0f;
        menu.fadeAmount = 0.1f;
        menu.fadeInAmount = 0.2f;
        menu.blurRadius = 2.0f;
        menu.onOpen = "play mouse_open; setfocus button_test;";
        menu.onClose = "play mouse_close;";
        menu.onESC = "close self;";
        menu.onKey = &menuKeyHandler;
        menu.visibleExp = {.numEntries = static_cast<int>(menuVisibleEntries.size()), .entries = menuVisibleEntries.data()};
        menu.allowedBinding = "+activate";
        menu.soundName = "menu_music";
        menu.focusColor[0] = 1.0f;
        menu.focusColor[1] = 0.8f;
        menu.focusColor[2] = 0.2f;
        menu.focusColor[3] = 1.0f;
        menu.itemCount = static_cast<int>(std::size(items));
        menu.items = items;

        Zone zone("MockZone", 0, GameId::T4, GamePlatform::PC);
        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<menuDef_t>>(ASSET_TYPE_MENU, menu.window.name, &menu));

        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        menu::MenuDumperT4 dumper;
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("ui_mp/test_menu.menu");
        REQUIRE(file);

        const auto output = file->AsString();
        constexpr auto expectedOutput = R"({
    menuDef
    {
        name                        "test_menu"
        fullscreen                  1
        rect                        0 0 640 480 0 0
        style                       1
        border                      1
        borderSize                  2
        backcolor                   0.1 0.2 0.3 0.75
        focuscolor                  1 0.8 0.2 1
        ownerdrawFlag               4
        soundLoop                   "menu_music"
        fadeClamp                   1
        fadeCycle                   16
        fadeAmount                  0.1
        fadeInAmount                0.2
        blurWorld                   2
        allowedBinding              "+activate"
        visible                     when(dvarbool("ui_test"));
        onOpen
        {
            play mouse_open;
            setfocus button_test;
        }
        onClose
        {
            play mouse_close;
        }
        onESC
        {
            close self;
        }
        execKeyInt 13
        {
            open key_help;
        }
        itemDef
        {
            name                        "button_test"
            text                        "@MENU_TEST"
            group                       "buttons"
            rect                        10 20 180 24 1 2
            style                       1
            type                        1
            visible                     when(localvarint("ui_highlight") == 5 && localvarstring("ui_choicegroup") == "popmenu");
            ownerdrawFlag               4096
            textalign                   10
            textalignx                  -6
            textscale                   0.4
            textstyle                   6
            textfont                    1
            backcolor                   0.1 0.1 0.1 0.25
            forecolor                   0.69 0.69 0.69 1
            background                  "background_material"
            onFocus
            {
                play mouse_over;
                setlocalvarint ui_highlight 5;
            }
            leaveFocus
            {
                setlocalvarint ui_highlight 0;
            }
            mouseExit
            {
                setitemcolor button_test bordercolor 0.1 0.1 0.12 0.5;
            }
            action
            {
                play mouse_click;
                open options;
            }
            execKey "a"
            {
                open advanced;
            }
            exp material                ("expression_material");
        }
        itemDef
        {
            name                        "name_field"
            text                        ""
            group                       "fields"
            rect                        10 56 180 24 1 2
            style                       1
            type                        18
            border                      1
            borderSize                  1
            visible                     1
            textalign                   1
            textalignx                  -28
            textaligny                  14
            textscale                   0.3
            textfont                    1
            backcolor                   0.1 0.1 0.1 0.25
            bordercolor                 0.5 0.5 0.5 1
            accept
            {
                setdvar ui_name player_name;
            }
            dvar                        "player_name"
            maxChars                    16
            maxPaintChars               16
        }
    }
}
)";
        REQUIRE(output == expectedOutput);

        menu::MenuAssetZoneState zoneState;
        std::istringstream input(output);
        menu::MenuFileReader reader(input, "ui_mp/test_menu.menu", menu::FeatureLevel::IW4, mockObjPath);
        reader.IncludeZoneState(zoneState);

        const auto parsed = reader.ReadMenuFile();
        REQUIRE(parsed);
        REQUIRE(parsed->m_menus.size() == 1);
        REQUIRE(parsed->m_menus[0]->m_name == "test_menu");
        REQUIRE(parsed->m_menus[0]->m_owner_draw_flags == UI_SHOW_FAVORITE_SERVERS);
        REQUIRE(parsed->m_menus[0]->m_items.size() == 2);
        REQUIRE(parsed->m_menus[0]->m_items[0]->m_name == "button_test");
        REQUIRE(parsed->m_menus[0]->m_items[0]->m_owner_draw_flags == UI_SHOW_NOT_FAVORITE_SERVERS);
        REQUIRE(parsed->m_menus[0]->m_items[1]->m_name == "name_field");
        REQUIRE(parsed->m_menus[0]->m_items[1]->m_dvar == "player_name");
    }

    TEST_CASE("MenuDumperT4: Prefers parent menu list path over ui_mp fallback", "[t4][menu][assetdumper]")
    {
        menuDef_t menu{};
        menu.window.name = "test_menu";

        menuDef_t* menus[]{&menu};

        MenuList menuList{};
        menuList.name = "ui/menus.txt";
        menuList.menuCount = static_cast<int>(std::size(menus));
        menuList.menus = menus;

        Zone zone("MockZone", 0, GameId::T4, GamePlatform::PC);
        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<menuDef_t>>(ASSET_TYPE_MENU, menu.window.name, &menu));
        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<MenuList>>(ASSET_TYPE_MENULIST, menuList.name, &menuList));

        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        menu::MenuListDumperT4 menuListDumper;
        menuListDumper.Dump(context);

        menu::MenuDumperT4 menuDumper;
        menuDumper.Dump(context);

        REQUIRE(mockOutput.GetMockedFile("ui/test_menu.menu"));
        REQUIRE_FALSE(mockOutput.GetMockedFile("ui_mp/test_menu.menu"));
    }

    TEST_CASE("MenuDumperT4: Expressions are valid when dumped", "[t4][menu][assetdumper]")
    {
        std::array item0VisibleValues{
            Operator(OP_LEFTPAREN),
            Operator(OP_LOCALVARSTRING),
            StringOperand("ui_highlight"),
            Operator(OP_RIGHTPAREN),
            Operator(OP_RIGHTPAREN),
        };
        auto item0Entries = EntryPointers(item0VisibleValues);

        itemDef_s item0{};
        item0.window.foreColor[0] = 1.0f;
        item0.window.foreColor[1] = 1.0f;
        item0.window.foreColor[2] = 1.0f;
        item0.window.foreColor[3] = 1.0f;
        item0.window.rectClient = {.x = 1, .y = 2, .w = 3, .h = 4, .horzAlign = 0, .vertAlign = 0};
        item0.textExp = {.numEntries = static_cast<int>(item0Entries.size()), .entries = item0Entries.data()};

        std::array item1VisibleValues{
            Operator(OP_LEFTPAREN),
            IntOperand(2),
            Operator(OP_RIGHTPAREN),
        };
        auto item1Entries = EntryPointers(item1VisibleValues);

        itemDef_s item1{};
        item1.window.foreColor[0] = 1.0f;
        item1.window.foreColor[1] = 1.0f;
        item1.window.foreColor[2] = 1.0f;
        item1.window.foreColor[3] = 1.0f;
        item1.window.rectClient = {.x = 1, .y = 2, .w = 3, .h = 4, .horzAlign = 0, .vertAlign = 0};
        item1.textExp = {.numEntries = static_cast<int>(item1Entries.size()), .entries = item1Entries.data()};

        std::array item2VisibleValues{
            Operator(OP_LEFTPAREN),
            IntOperand(3),
            Operator(OP_ADD),
            IntOperand(7),
            Operator(OP_RIGHTPAREN),
        };
        auto item2Entries = EntryPointers(item2VisibleValues);

        itemDef_s item2{};
        item2.window.foreColor[0] = 1.0f;
        item2.window.foreColor[1] = 1.0f;
        item2.window.foreColor[2] = 1.0f;
        item2.window.foreColor[3] = 1.0f;
        item2.window.rectClient = {.x = 1, .y = 2, .w = 3, .h = 4, .horzAlign = 0, .vertAlign = 0};
        item2.textExp = {.numEntries = static_cast<int>(item2Entries.size()), .entries = item2Entries.data()};

        std::array item3VisibleValues{
            Operator(OP_LEFTPAREN),
            IntOperand(-3),
            Operator(OP_ADD),
            IntOperand(7),
            Operator(OP_MULTIPLY),
            Operator(OP_NOT),
            Operator(OP_SIN),
            Operator(OP_COS),
            IntOperand(7),
            Operator(OP_RIGHTPAREN),
            Operator(OP_RIGHTPAREN),
            Operator(OP_RIGHTPAREN),
        };
        auto item3Entries = EntryPointers(item3VisibleValues);

        itemDef_s item3{};
        item3.window.foreColor[0] = 1.0f;
        item3.window.foreColor[1] = 1.0f;
        item3.window.foreColor[2] = 1.0f;
        item3.window.foreColor[3] = 1.0f;
        item3.window.rectClient = {.x = 1, .y = 2, .w = 3, .h = 4, .horzAlign = 0, .vertAlign = 0};
        item3.textExp = {.numEntries = static_cast<int>(item3Entries.size()), .entries = item3Entries.data()};

        itemDef_s* items[]{&item0, &item1, &item2, &item3};

        menuDef_t menu{};
        menu.window.name = "test_menu";
        menu.window.foreColor[0] = 1.0f;
        menu.window.foreColor[1] = 1.0f;
        menu.window.foreColor[2] = 1.0f;
        menu.window.foreColor[3] = 1.0f;
        menu.window.rect = {.x = 1, .y = 2, .w = 3, .h = 4, .horzAlign = 0, .vertAlign = 0};
        menu.itemCount = static_cast<int>(std::size(items));
        menu.items = items;

        Zone zone("MockZone", 0, GameId::T4, GamePlatform::PC);
        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<menuDef_t>>(ASSET_TYPE_MENU, menu.window.name, &menu));

        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        menu::MenuDumperT4 dumper;
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("ui_mp/test_menu.menu");
        REQUIRE(file);

        const auto output = file->AsString();
        constexpr auto expectedOutput = R"({
    menuDef
    {
        name                        "test_menu"
        rect                        1 2 3 4 0 0
        itemDef
        {
            rect                        1 2 3 4 0 0
            exp text                    (localvarstring("ui_highlight"));
        }
        itemDef
        {
            rect                        1 2 3 4 0 0
            exp text                    (2);
        }
        itemDef
        {
            rect                        1 2 3 4 0 0
            exp text                    (3 + 7);
        }
        itemDef
        {
            rect                        1 2 3 4 0 0
            exp text                    (-3 + 7 * !sin(cos(7)));
        }
    }
}
)";
        REQUIRE(output == expectedOutput);
    }

    TEST_CASE("MenuDumperT4: Can dump T4-specific menu properties", "[t4][menu][assetdumper]")
    {
        std::array visibleValues{
            Operator(OP_PARTYISMISSINGMAPPACK),
            Operator(OP_RIGHTPAREN),
        };
        auto visibleEntries = EntryPointers(visibleValues);

        Material selectIcon{};
        selectIcon.info.name = "listbox_select_icon";

        Material listboxBackground{};
        listboxBackground.info.name = "listbox_background";

        Material highlightTexture{};
        highlightTexture.info.name = "listbox_highlight";

        listBoxDef_s listBox{};
        listBox.onDoubleClick = "play mouse_click;";
        listBox.focusColor[0] = 1.0f;
        listBox.focusColor[1] = 0.8f;
        listBox.focusColor[2] = 0.2f;
        listBox.focusColor[3] = 1.0f;
        listBox.selectIcon = &selectIcon;
        listBox.backgroundItemListbox = &listboxBackground;
        listBox.highlightTexture = &highlightTexture;

        itemDef_s serverList{};
        serverList.window.name = "server_list";
        serverList.window.rectClient = {.x = 20.0f, .y = 80.0f, .w = 400.0f, .h = 180.0f, .horzAlign = 1, .vertAlign = 2};
        serverList.window.foreColor[0] = 1.0f;
        serverList.window.foreColor[1] = 1.0f;
        serverList.window.foreColor[2] = 1.0f;
        serverList.window.foreColor[3] = 1.0f;
        serverList.type = ITEM_TYPE_LISTBOX;
        serverList.onListboxSelectionChange = "setdvar ui_server_selected 1;";
        serverList.typeData.listBox = &listBox;

        itemDef_s* items[]{&serverList};

        menuDef_t menu{};
        menu.window.name = "t4_menu";
        menu.window.rect = {.x = 0.0f, .y = 0.0f, .w = 640.0f, .h = 480.0f, .horzAlign = 0, .vertAlign = 0};
        menu.window.foreColor[0] = 1.0f;
        menu.window.foreColor[1] = 1.0f;
        menu.window.foreColor[2] = 1.0f;
        menu.window.foreColor[3] = 1.0f;
        menu.onFocus = "setdvar ui_hint t4_menu;";
        menu.visibleExp = {.numEntries = static_cast<int>(visibleEntries.size()), .entries = visibleEntries.data()};
        menu.itemCount = static_cast<int>(std::size(items));
        menu.items = items;

        Zone zone("MockZone", 0, GameId::T4, GamePlatform::PC);
        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<menuDef_t>>(ASSET_TYPE_MENU, menu.window.name, &menu));

        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        menu::MenuDumperT4 dumper;
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("ui_mp/t4_menu.menu");
        REQUIRE(file);

        constexpr auto expectedOutput = R"({
    menuDef
    {
        name                        "t4_menu"
        rect                        0 0 640 480 0 0
        visible                     when(partyismissingmappack());
        onFocus
        {
            setdvar ui_hint t4_menu;
        }
        itemDef
        {
            name                        "server_list"
            rect                        20 80 400 180 1 2
            type                        6
            doubleclick
            {
                play mouse_click;
            }
            onListboxSelectionChange
            {
                setdvar ui_server_selected 1;
            }
            focusColor                  1 0.8 0.2 1
            selectIcon                  "listbox_select_icon"
            backgroundItemListbox       "listbox_background"
            highlightTexture            "listbox_highlight"
        }
    }
}
)";
        REQUIRE(file->AsString() == expectedOutput);
    }
} // namespace
