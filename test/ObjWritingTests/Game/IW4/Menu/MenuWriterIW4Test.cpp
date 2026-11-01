#include "Game/IW4/Menu/MenuWriterIW4.h"

#include "Game/IW4/IW4.h"

#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <string>

using namespace IW4;

namespace
{
    TEST_CASE("MenuWriterIW4: Writes the dvar of a text item without edit field data", "[iw4][menu]")
    {
        itemDef_s item{};
        item.window.name = "tip";
        item.type = ITEM_TYPE_TEXT;
        item.dvar = "didyouknow";

        itemDef_s* items[]{&item};

        menuDef_t menu{};
        menu.window.name = "connect";
        menu.items = items;
        menu.itemCount = 1;

        std::ostringstream ss;
        const auto writer = menu::CreateMenuWriterIW4(ss);
        writer->Start();
        writer->WriteMenu(menu);
        writer->End();

        REQUIRE(ss.str().find("dvar                        \"didyouknow\"") != std::string::npos);
    }
} // namespace
