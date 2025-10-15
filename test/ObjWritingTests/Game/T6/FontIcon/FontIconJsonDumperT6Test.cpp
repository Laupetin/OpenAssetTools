#include "Game/T6/FontIcon/FontIconJsonDumperT6.h"

#include "Asset/AssetRegistration.h"
#include "Game/T6/CommonT6.h"
#include "Game/T6/GameT6.h"
#include "NormalizedJson.h"
#include "Pool/AssetPoolDynamic.h"
#include "SearchPath/MockOutputPath.h"
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
    Material* GivenMaterial(const std::string& name, MemoryManager& memory)
    {
        auto* material = memory.Alloc<Material>();
        material->info.name = memory.Dup(name.c_str());

        return material;
    }

    void GivenFontIcon(const std::string& name, AssetPool<FontIcon>& pool, MemoryManager& memory)
    {
        auto* fontIcon = memory.Alloc<FontIcon>();
        fontIcon->name = memory.Dup(name.c_str());

        fontIcon->numEntries = 3;
        fontIcon->fontIconEntry = memory.Alloc<FontIconEntry>(fontIcon->numEntries);

        auto& entry0 = fontIcon->fontIconEntry[0];
        entry0.fontIconName.string = "XenonButtondpadAll";
        entry0.fontIconName.hash = Common::Com_HashString(entry0.fontIconName.string);
        entry0.fontIconMaterialHandle = GivenMaterial("xenonbutton_dpad_all", memory);
        entry0.fontIconSize = 32;
        entry0.xScale = 1.0f;
        entry0.yScale = 1.0f;

        auto& entry1 = fontIcon->fontIconEntry[1];
        entry1.fontIconName.string = "XenonButtonLStickAnimatedD";
        entry1.fontIconName.hash = Common::Com_HashString(entry1.fontIconName.string);
        entry1.fontIconMaterialHandle = GivenMaterial("ui_button_xenon_lstick_anim_d", memory);
        entry1.fontIconSize = 32;
        entry1.xScale = 1.5f;
        entry1.yScale = 1.5f;

        auto& entry2 = fontIcon->fontIconEntry[2];
        entry2.fontIconName.string = "XenonButtonStickAnimatedL";
        entry2.fontIconName.hash = Common::Com_HashString(entry2.fontIconName.string);
        entry2.fontIconMaterialHandle = GivenMaterial("xenonbutton_ls", memory);
        entry2.fontIconSize = 32;
        entry2.xScale = 1.0f;
        entry2.yScale = 1.0f;

        fontIcon->numAliasEntries = 6;
        fontIcon->fontIconAlias = memory.Alloc<FontIconAlias>(fontIcon->numAliasEntries);

        auto& alias0 = fontIcon->fontIconAlias[0];
        alias0.aliasHash = Common::Com_HashString("BUTTON_LUI_DPAD_ALL");
        alias0.buttonHash = entry0.fontIconName.hash;

        auto& alias1 = fontIcon->fontIconAlias[1];
        alias1.aliasHash = static_cast<int>(0xCE7211DA);
        alias1.buttonHash = entry1.fontIconName.hash;

        auto& alias2 = fontIcon->fontIconAlias[2];
        alias2.aliasHash = Common::Com_HashString("BUTTON_MOVE");
        alias2.buttonHash = entry2.fontIconName.hash;

        auto& alias3 = fontIcon->fontIconAlias[3];
        alias3.aliasHash = Common::Com_HashString("BUTTON_EMBLEM_MOVE");
        alias3.buttonHash = entry2.fontIconName.hash;

        auto& alias4 = fontIcon->fontIconAlias[4];
        alias4.aliasHash = Common::Com_HashString("BUTTON_LUI_LEFT_STICK_UP");
        alias4.buttonHash = entry2.fontIconName.hash;

        auto& alias5 = fontIcon->fontIconAlias[5];
        alias5.aliasHash = Common::Com_HashString("BUTTON_MOVESTICK");
        alias5.buttonHash = entry2.fontIconName.hash;

        pool.AddAsset(std::make_unique<XAssetInfo<FontIcon>>(ASSET_TYPE_FONTICON, name, fontIcon));
    }

    TEST_CASE("DumperFontIconJson(T6): Can dump font icon", "[t6][font-icon][assetdumper]")
    {
        std::string expected(R"FONT_ICON(
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

        Zone zone("MockZone", 0, GameId::T6);

        MemoryManager memory;
        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        AssetPoolDynamic<FontIcon> fontIconPool(0);
        GivenFontIcon("fonticon/test.csv", fontIconPool, memory);

        font_icon::JsonDumperT6 dumper(fontIconPool);
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("fonticon/test.json");
        REQUIRE(file);
        REQUIRE(JsonNormalized(file->AsString()) == JsonNormalized(expected));
    }
} // namespace
