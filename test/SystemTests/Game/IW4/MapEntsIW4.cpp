#include "Game/IW4/Maps/MapEntsDumperIW4.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"
#include "ZoneLoading.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>

using namespace IW4;

namespace
{
    TEST_CASE("MapEnts dumper writes entity string (IW4)", "[iw4][mapents][system]")
    {
        constexpr auto MAP_ENTS_NAME = "maps/mp/test_map.d3dbsp";
        char entityString[] = R"({
"classname" "worldspawn"
}
)";

        TriggerModel triggerModel{};
        Stage stage{};
        MapEnts mapEnts{};
        mapEnts.name = MAP_ENTS_NAME;
        mapEnts.entityString = entityString;
        mapEnts.numEntityChars = sizeof(entityString);
        mapEnts.trigger.count = 1;
        mapEnts.trigger.models = &triggerModel;
        mapEnts.stages = &stage;
        mapEnts.stageCount = 1;

        Zone zone("MockZone", 0, GameId::IW4, GamePlatform::PC);
        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<MapEnts>>(ASSET_TYPE_MAP_ENTS, mapEnts.name, &mapEnts));

        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext dumpingContext(zone, "", mockOutput, mockObjPath, std::nullopt);
        map_ents::DumperIW4 dumper;
        dumper.Dump(dumpingContext);

        const auto* dumpedFile = mockOutput.GetMockedFile("maps/mp/test_map.d3dbsp.ents");
        REQUIRE(dumpedFile != nullptr);
        REQUIRE(dumpedFile->m_data.size() == sizeof(entityString) - 1);
        REQUIRE(dumpedFile->AsString() == entityString);
    }
} // namespace
