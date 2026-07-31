
#include "Game/T6/PhysPreset/PhysPresetInfoStringDumperT6.h"

#include "Game/T6/ObjConstantsT6.h"
#include "InfoString/InfoString.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace T6;
using namespace Catch;

namespace
{
    std::unique_ptr<PhysPreset> CreateTestPhysPreset()
    {
        auto physPreset = std::make_unique<PhysPreset>();

        physPreset->name = "test_phys_preset";
        physPreset->mass = 1.25f;
        physPreset->bounce = 0.25f;
        physPreset->friction = PHYS_PRESET_MAX_FRICTION;
        physPreset->bulletForceScale = 0.5f;
        physPreset->explosiveForceScale = 0.75f;
        physPreset->piecesSpreadFraction = 0.125f;
        physPreset->piecesUpwardVelocity = 64.0f;
        physPreset->canFloat = 1;
        physPreset->gravityScale = 2.0f;
        physPreset->centerOfMassOffset.x = 1.5f;
        physPreset->centerOfMassOffset.y = 2.5f;
        physPreset->centerOfMassOffset.z = 3.5f;
        physPreset->buoyancyBoxMin.x = 4.5f;
        physPreset->buoyancyBoxMin.y = 5.5f;
        physPreset->buoyancyBoxMin.z = 6.5f;
        physPreset->buoyancyBoxMax.x = 7.5f;
        physPreset->buoyancyBoxMax.y = 8.5f;
        physPreset->buoyancyBoxMax.z = 9.5f;

        return physPreset;
    }

    std::string DumpPhysPreset(PhysPreset& physPreset)
    {
        Zone dumpingZone("DumpingZone", 0, GameId::T6, GamePlatform::PC);
        dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<PhysPreset>>(ASSET_TYPE_PHYSPRESET, physPreset.name, &physPreset));

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        phys_preset::InfoStringDumperT6 dumper;
        dumper.Dump(dumpingContext);

        const auto* dumpedFile = dumpingOutput.GetMockedFile("physic/test_phys_preset");
        REQUIRE(dumpedFile != nullptr);
        return dumpedFile->AsString();
    }

    TEST_CASE("PhysPreset dumper serializes raw fields (T6)", "[t6][physpreset][system]")
    {
        const auto testPhysPreset = CreateTestPhysPreset();
        std::istringstream dumpedStream(DumpPhysPreset(*testPhysPreset));

        InfoString infoString;
        REQUIRE(infoString.FromStream(INFO_STRING_PREFIX_PHYS_PRESET, dumpedStream));
        REQUIRE(infoString.GetValueForKey("mass") == "1250");
        REQUIRE(infoString.GetValueForKey("bounce") == "0.25");
        REQUIRE(infoString.GetValueForKey("friction") == "0");
        REQUIRE(infoString.GetValueForKey("isFrictionInfinity") == "1");
        REQUIRE(infoString.GetValueForKey("bulletForceScale") == "0.5");
        REQUIRE(infoString.GetValueForKey("explosiveForceScale") == "0.75");
        REQUIRE(infoString.GetValueForKey("piecesSpreadFraction") == "0.125");
        REQUIRE(infoString.GetValueForKey("piecesUpwardVelocity") == "64");
        REQUIRE(infoString.GetValueForKey("canFloat") == "1");
        REQUIRE(infoString.GetValueForKey("gravityScale") == "2");
        REQUIRE(infoString.GetValueForKey("massOffsetX") == "1.5");
        REQUIRE(infoString.GetValueForKey("massOffsetY") == "2.5");
        REQUIRE(infoString.GetValueForKey("massOffsetZ") == "3.5");
        REQUIRE(infoString.GetValueForKey("buoyancyMinX") == "4.5");
        REQUIRE(infoString.GetValueForKey("buoyancyMinY") == "5.5");
        REQUIRE(infoString.GetValueForKey("buoyancyMinZ") == "6.5");
        REQUIRE(infoString.GetValueForKey("buoyancyMaxX") == "7.5");
        REQUIRE(infoString.GetValueForKey("buoyancyMaxY") == "8.5");
        REQUIRE(infoString.GetValueForKey("buoyancyMaxZ") == "9.5");
    }
} // namespace
