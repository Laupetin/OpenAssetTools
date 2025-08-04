#include "Game/T6/FontIcon/JsonLoaderFontIconT6.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/GameT6.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace T6;
using namespace Catch;
using namespace std::literals;

namespace
{
    void GivenMaterial(const std::string& name, AssetCreationContext& context, MemoryManager& memory)
    {
        auto* material = memory.Alloc<Material>();
        material->info.name = memory.Dup(name.c_str());

        AssetRegistration<AssetMaterial> registration(name);
        registration.SetAsset(material);
        context.AddAsset(std::move(registration));
    }

    TEST_CASE("JsonLoaderFontIcon(T6): Can parse font icon", "[t6][font-icon][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("fonticon/test.json",
                               R"FONT_ICON(
{
    "$schema": "http://openassettools.dev/schema/font-icon.v1.json",
    "_game": "t6",
    "_type": "font-icon",
    "_version": 1,
    "entries": [
        {
            "aliases": [
                "BUTTON_LUI_DPAD_ALL"
            ],
            "material": "xenonbutton_dpad_all",
            "name": "XenonButtondpadAll",
            "scale": {
                "x": 1.0,
                "y": 1.0
            },
            "size": 32
        },
        {
            "aliasHashes": [
                3463582170
            ],
            "aliases": [],
            "material": "ui_button_xenon_lstick_anim_d",
            "name": "XenonButtonLStickAnimatedD",
            "scale": {
                "x": 1.5,
                "y": 1.5
            },
            "size": 32
        },
        {
            "aliases": [
                "BUTTON_MOVE",
                "BUTTON_EMBLEM_MOVE",
                "BUTTON_LUI_LEFT_STICK_UP",
                "BUTTON_MOVESTICK"
            ],
            "material": "xenonbutton_ls",
            "name": "XenonButtonStickAnimatedL",
            "scale": {
                "x": 1.0,
                "y": 1.0
            },
            "size": 32
        }
    ]
})FONT_ICON");

        Zone zone("MockZone", 0, IGame::GetGameById(GameId::T6));

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        GivenMaterial("xenonbutton_dpad_all", context, memory);
        GivenMaterial("ui_button_xenon_lstick_anim_d", context, memory);
        GivenMaterial("xenonbutton_ls", context, memory);

        auto loader = font_icon::CreateJsonLoader(memory, searchPath);
        auto result = loader->CreateAsset("fonticon/test.csv", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<FontIcon>*>(result.GetAssetInfo());
        const auto* fontIcon = assetInfo->Asset();

        REQUIRE(fontIcon->name == "fonticon/test.csv"s);

        REQUIRE(fontIcon->numEntries == 3);
        REQUIRE(fontIcon->fontIconEntry != nullptr);

        const auto& entry0 = fontIcon->fontIconEntry[0];
        REQUIRE(entry0.fontIconName.string == "XenonButtondpadAll"s);
        REQUIRE(entry0.fontIconName.hash == static_cast<int>(0x9220177B));
        REQUIRE(entry0.fontIconMaterialHandle->info.name == "xenonbutton_dpad_all"s);
        REQUIRE(entry0.fontIconSize == 32);
        REQUIRE(entry0.xScale == 1.0f);
        REQUIRE(entry0.yScale == 1.0f);

        const auto& entry1 = fontIcon->fontIconEntry[1];
        REQUIRE(entry1.fontIconName.string == "XenonButtonLStickAnimatedD"s);
        REQUIRE(entry1.fontIconName.hash == static_cast<int>(0x9E7D535A));
        REQUIRE(entry1.fontIconMaterialHandle->info.name == "ui_button_xenon_lstick_anim_d"s);
        REQUIRE(entry1.fontIconSize == 32);
        REQUIRE(entry1.xScale == 1.5f);
        REQUIRE(entry1.yScale == 1.5f);

        const auto& entry2 = fontIcon->fontIconEntry[2];
        REQUIRE(entry2.fontIconName.string == "XenonButtonStickAnimatedL"s);
        REQUIRE(entry2.fontIconName.hash == static_cast<int>(0xABF58CD6));
        REQUIRE(entry2.fontIconMaterialHandle->info.name == "xenonbutton_ls"s);
        REQUIRE(entry2.fontIconSize == 32);
        REQUIRE(entry2.xScale == 1.0f);
        REQUIRE(entry2.yScale == 1.0f);

        REQUIRE(fontIcon->numAliasEntries == 6);
        REQUIRE(fontIcon->fontIconAlias != nullptr);

        auto& alias0 = fontIcon->fontIconAlias[0];
        REQUIRE(alias0.aliasHash == static_cast<int>(0xCE7211DA));
        REQUIRE(alias0.buttonHash == entry1.fontIconName.hash);

        const auto& alias1 = fontIcon->fontIconAlias[1];
        REQUIRE(alias1.aliasHash == static_cast<int>(0xDD3B363A));
        REQUIRE(alias1.buttonHash == entry0.fontIconName.hash);

        const auto& alias2 = fontIcon->fontIconAlias[2];
        REQUIRE(alias2.aliasHash == static_cast<int>(0xFFFBB17));
        REQUIRE(alias2.buttonHash == entry2.fontIconName.hash);

        const auto& alias3 = fontIcon->fontIconAlias[3];
        REQUIRE(alias3.aliasHash == static_cast<int>(0x1893B6A8));
        REQUIRE(alias3.buttonHash == entry2.fontIconName.hash);

        const auto& alias4 = fontIcon->fontIconAlias[4];
        REQUIRE(alias4.aliasHash == static_cast<int>(0x18A33DB5));
        REQUIRE(alias4.buttonHash == entry2.fontIconName.hash);

        const auto& alias5 = fontIcon->fontIconAlias[5];
        REQUIRE(alias5.aliasHash == static_cast<int>(0x5A15AB35));
        REQUIRE(alias5.buttonHash == entry2.fontIconName.hash);
    }
} // namespace
