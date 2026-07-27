#include "Game/T4/XModel/XModelCollMapDumperT4.h"

#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>
#include <string_view>

using namespace T4;

namespace
{
    size_t CountOccurrences(const std::string_view value, const std::string_view searchValue)
    {
        auto count = 0u;
        auto position = 0u;

        while ((position = value.find(searchValue, position)) != std::string_view::npos)
        {
            count++;
            position += searchValue.size();
        }

        return count;
    }

    TEST_CASE("XModelCollMapDumperT4: Dumps collision_wall_32x32x10 brush", "[t4][xmodel][collmap][assetdumper]")
    {
        BrushWrapper brush{};
        brush.mins[0] = -16.0f;
        brush.mins[1] = -5.0f;
        brush.mins[2] = -16.0f;
        brush.maxs[0] = 16.0f;
        brush.maxs[1] = 5.0f;
        brush.maxs[2] = 16.0f;
        for (auto& edgeCounts : brush.edgeCount)
        {
            for (auto& edgeCount : edgeCounts)
                edgeCount = 4;
        }

        PhysGeomInfo geom{};
        geom.brush = &brush;
        geom.type = PHYS_GEOM_NONE;

        PhysGeomList collMap{};
        collMap.count = 1u;
        collMap.geoms = &geom;

        Zone zone("common_mp", 0, GameId::T4, GamePlatform::PC);
        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        xmodel::DumpXModelCollMapT4("collision_wall_32x32x10", context, &collMap);

        const auto* file = mockOutput.GetMockedFile("collmaps/collision_wall_32x32x10.map");
        REQUIRE(file != nullptr);

        constexpr auto expected = R"(iwmap 4
"000_Global" flags active
"The Map" flags
// entity 0
{
  "classname" "worldspawn"
  // brush 0
  {
    ( -16 0 0 ) ( -16 64 0 ) ( -16 0 64 ) clip_player 64 64 0 0 0 0 lightmap_gray 16384 16384 0 0 0 0
    ( 16 0 0 ) ( 16 -64 0 ) ( 16 0 64 ) clip_player 64 64 0 0 0 0 lightmap_gray 16384 16384 0 0 0 0
    ( 0 -5 0 ) ( -64 -5 0 ) ( 0 -5 64 ) clip_player 64 64 0 0 0 0 lightmap_gray 16384 16384 0 0 0 0
    ( 0 5 0 ) ( 64 5 0 ) ( 0 5 64 ) clip_player 64 64 0 0 0 0 lightmap_gray 16384 16384 0 0 0 0
    ( 0 0 -16 ) ( 64 0 -16 ) ( 0 64 -16 ) clip_player 64 64 0 0 0 0 lightmap_gray 16384 16384 0 0 0 0
    ( 0 0 16 ) ( -64 0 16 ) ( 0 64 16 ) clip_player 64 64 0 0 0 0 lightmap_gray 16384 16384 0 0 0 0
  }
}
)";
        REQUIRE(file->AsString() == expected);
    }

    TEST_CASE("XModelCollMapDumperT4: Dumps collision_geo_ramp without a false axial face", "[t4][xmodel][collmap][assetdumper]")
    {
        constexpr auto slopeLength = 50.249378f;

        cplane_s slope{};
        slope.normal[1] = 26.0f / slopeLength;
        slope.normal[2] = 43.0f / slopeLength;
        slope.dist = 576.0f / slopeLength;

        cbrushside_t side{};
        side.plane = &slope;

        BrushWrapper brush{};
        brush.mins[0] = -128.0f;
        brush.mins[1] = -53.0f;
        brush.mins[2] = -12.0f;
        brush.maxs[0] = 127.0f;
        brush.maxs[1] = 42.0f;
        brush.maxs[2] = 14.0f;
        brush.edgeCount[0][0] = 4;
        brush.edgeCount[0][1] = 4;
        brush.edgeCount[0][2] = 4;
        brush.edgeCount[1][0] = 4;
        brush.edgeCount[1][2] = 4;
        brush.numsides = 1u;
        brush.sides = &side;

        PhysGeomInfo geom{};
        geom.brush = &brush;
        geom.type = PHYS_GEOM_NONE;

        PhysGeomList collMap{};
        collMap.count = 1u;
        collMap.geoms = &geom;

        Zone zone("common_mp", 0, GameId::T4, GamePlatform::PC);
        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        xmodel::DumpXModelCollMapT4("collision_geo_ramp", context, &collMap);

        const auto* file = mockOutput.GetMockedFile("collmaps/collision_geo_ramp.map");
        REQUIRE(file != nullptr);

        const auto output = file->AsString();
        REQUIRE(CountOccurrences(output, "clip_player") == 6u);
        REQUIRE(output.find("( 0 -53 0 )") != std::string_view::npos);
        REQUIRE(output.find("( 0 42 0 )") == std::string_view::npos);
        REQUIRE(output.find(") ( 64 5.") != std::string_view::npos);
    }
} // namespace
