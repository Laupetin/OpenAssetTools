#include "Game/T5/Zone/Definition/ZoneDefWriterT5.h"

#include "Game/T5/T5.h"
#include "Pool/XAssetInfo.h"

#include <catch2/catch_test_macros.hpp>
#include <sstream>

using namespace T5;

namespace
{
    std::string WriteImpactTableZoneDef(const std::string& zoneName, const std::string& assetName)
    {
        Zone zone(zoneName, 0, GameId::T5, GamePlatform::PC);
        zone.m_pools.AddAsset(std::make_unique<XAssetInfoGeneric>(ASSET_TYPE_IMPACT_FX, assetName, nullptr));

        std::ostringstream output;
        ZoneDefWriter writer;
        writer.WriteZoneDef(output, zone, false, false);
        return output.str();
    }

    TEST_CASE("T5 zone definitions reconstruct unnamed impact table names", "[t5][zonedefinition]")
    {
        SECTION("A map table uses the zone name")
        {
            REQUIRE(WriteImpactTableZoneDef("mp_array", "").find("fximpacttable,mp_array\n") != std::string::npos);
        }

        SECTION("A common table uses the void source name")
        {
            REQUIRE(WriteImpactTableZoneDef("common", "").find("fximpacttable,void\n") != std::string::npos);
        }

        SECTION("A multiplayer common table uses the mp_maps source name")
        {
            REQUIRE(WriteImpactTableZoneDef("common_mp", "").find("fximpacttable,mp_maps\n") != std::string::npos);
        }

        SECTION("The retail ImpactFx placeholder uses the mp_maps source name")
        {
            REQUIRE(WriteImpactTableZoneDef("common_mp", "ImpactFx").find("fximpacttable,mp_maps\n") != std::string::npos);
        }

        SECTION("A zombie common table uses the zombie source name")
        {
            REQUIRE(WriteImpactTableZoneDef("common_zombie", "").find("fximpacttable,zombie\n") != std::string::npos);
        }

        SECTION("A named table keeps its name")
        {
            REQUIRE(WriteImpactTableZoneDef("common_mp", "mp_maps").find("fximpacttable,mp_maps\n") != std::string::npos);
        }
    }
} // namespace
