#include "Game/IW4/PhysCollmap/PhysCollmapDumperIW4.h"

#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>

using namespace IW4;

namespace
{
    TEST_CASE("PhysCollmap dumper serializes com_junktire_colmap as physics_cylinder (IW4)", "[iw4][physcollmap][assetdumper]")
    {
        PhysGeomInfo geom{};
        geom.type = PHYS_GEOM_COLLMAP;
        geom.orientation[0][0] = -0.0f;
        geom.orientation[0][1] = 1.0f;
        geom.orientation[0][2] = -0.0f;
        geom.bounds.midPoint.v[0] = 0.0f;
        geom.bounds.midPoint.v[1] = 0.0f;
        geom.bounds.midPoint.v[2] = -0.0f;
        geom.bounds.halfSize.v[0] = 3.5f;
        geom.bounds.halfSize.v[1] = 11.947635f;
        geom.bounds.halfSize.v[2] = 0.0f;

        PhysCollmap physCollmap{};
        physCollmap.name = "com_junktire_colmap";
        physCollmap.count = 1;
        physCollmap.geoms = &geom;

        Zone zone("MockZone", 0, GameId::IW4, GamePlatform::PC);
        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<PhysCollmap>>(ASSET_TYPE_PHYSCOLLMAP, physCollmap.name, &physCollmap));

        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        phys_collmap::DumperIW4 dumper;
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("phys_collmaps/com_junktire_colmap.map");
        REQUIRE(file != nullptr);
        REQUIRE(file->AsString() == R"(iwmap 4
"000_Global" flags active
"The Map" flags
// entity 0
{
  "classname" "worldspawn"
  // brush 0
  {
    physics_cylinder
    {
      -0.000000 1.000000 -0.000000 0.000000 0.000000 -0.000000 7.000000 11.947635
    }
  }
}
)");
    }
} // namespace
