#include "Game/IW3/Zone/Definition/ZoneDefWriterIW3.h"

#include "Game/IW3/IW3.h"
#include "Pool/XAssetInfo.h"

#include <catch2/catch_test_macros.hpp>
#include <sstream>

using namespace IW3;

namespace
{
    std::string WriteImpactTableZoneDef(const std::string& zoneName, const std::string& assetName)
    {
        Zone zone(zoneName, 0, GameId::IW3, GamePlatform::PC);
        zone.m_pools.AddAsset(std::make_unique<XAssetInfoGeneric>(ASSET_TYPE_IMPACT_FX, assetName, nullptr));

        std::ostringstream output;
        ZoneDefWriter writer;
        writer.WriteZoneDef(output, zone, false, false);
        return output.str();
    }

    TEST_CASE("IW3 zone definitions reconstruct unnamed impact table names", "[iw3][zonedefinition]")
    {
        SECTION("A map table uses the zone name")
        {
            REQUIRE(WriteImpactTableZoneDef("mp_backlot", "").find("impactfx,mp_backlot\n") != std::string::npos);
        }

        SECTION("A common table uses the void source name")
        {
            REQUIRE(WriteImpactTableZoneDef("common_mp", "").find("impactfx,void\n") != std::string::npos);
        }

        SECTION("A named table keeps its name")
        {
            REQUIRE(WriteImpactTableZoneDef("code_post_gfx_mp", "default").find("impactfx,default\n") != std::string::npos);
        }
    }
} // namespace
