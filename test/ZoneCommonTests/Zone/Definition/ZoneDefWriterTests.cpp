#include "Game/IW4/IW4.h"
#include "Zone/Definition/ZoneDefWriter.h"
#include "Zone/Zone.h"

#include <catch2/catch_test_macros.hpp>
#include <sstream>

using namespace IW4;

namespace test::zone::definition::zone_def_writer
{
    void AddAssets(Zone& zone)
    {
        zone.m_pools.AddAsset(ASSET_TYPE_LOCALIZE_ENTRY, "MENU_TEST", nullptr, {}, {}, {});
        zone.m_pools.AddAsset(ASSET_TYPE_IMAGE, "unused_image", nullptr, {}, {}, {});
        auto* material = zone.m_pools.AddAsset(ASSET_TYPE_MATERIAL, "white", nullptr, {}, {}, {});
        auto* reference = zone.m_pools.AddAsset(ASSET_TYPE_MATERIAL, ",black", nullptr, {}, {}, {});
        auto* menu = zone.m_pools.AddAsset(ASSET_TYPE_MENU, "main", nullptr, {material, reference}, {}, {});
        zone.m_pools.AddAsset(ASSET_TYPE_MENULIST, "ui_mp/test.txt", nullptr, {menu}, {}, {});
    }

    std::string WriteZoneDef(const Zone& zone, std::vector<bool> assetTypes)
    {
        std::ostringstream ss;
        IZoneDefWriter::GetZoneDefWriterForGame(GameId::IW4)->WriteZoneDef(ss, zone, false, false, std::move(assetTypes));

        return ss.str();
    }

    TEST_CASE("ZoneDefWriter: Writes every asset without asset types", "[zonedefinition]")
    {
        Zone zone("test", 0, GameId::IW4, GamePlatform::PC);
        AddAssets(zone);

        REQUIRE(WriteZoneDef(zone, {}) == R"(// Call Of Duty: Modern Warfare 2
>game,IW4

localize,test
image,unused_image
material,white
material,,black
menu,main
menulist,ui_mp/test.txt
)");
    }

    TEST_CASE("ZoneDefWriter: Writes the dependencies of the specified asset types as references", "[zonedefinition]")
    {
        Zone zone("test", 0, GameId::IW4, GamePlatform::PC);
        AddAssets(zone);

        std::vector<bool> assetTypes(ASSET_TYPE_COUNT, false);
        assetTypes[ASSET_TYPE_MENULIST] = true;
        assetTypes[ASSET_TYPE_MENU] = true;

        REQUIRE(WriteZoneDef(zone, std::move(assetTypes)) == R"(// Call Of Duty: Modern Warfare 2
>game,IW4

material,,white
material,,black
menu,main
menulist,ui_mp/test.txt
)");
    }
} // namespace test::zone::definition::zone_def_writer
