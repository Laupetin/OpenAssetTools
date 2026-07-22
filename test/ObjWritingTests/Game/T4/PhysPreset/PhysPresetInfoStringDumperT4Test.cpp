
#include "Game/T4/PhysPreset/PhysPresetInfoStringDumperT4.h"

#include "Game/T4/ObjConstantsT4.h"
#include "InfoString/InfoString.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>

using namespace T4;

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
        physPreset->sndAliasPrefix = "physics_metal";
        physPreset->piecesSpreadFraction = 0.125f;
        physPreset->piecesUpwardVelocity = 64.0f;
        physPreset->canFloat = 1;
        physPreset->gravityScale = 2.0f;

        return physPreset;
    }

    std::string DumpPhysPreset(PhysPreset& physPreset)
    {
        Zone dumpingZone("DumpingZone", 0, GameId::T4, GamePlatform::PC);
        dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<PhysPreset>>(ASSET_TYPE_PHYSPRESET, physPreset.name, &physPreset));

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        phys_preset::InfoStringDumperT4 dumper;
        dumper.Dump(dumpingContext);

        const auto* dumpedFile = dumpingOutput.GetMockedFile("physic/test_phys_preset");
        REQUIRE(dumpedFile != nullptr);
        return dumpedFile->AsString();
    }

    TEST_CASE("PhysPreset dumper serializes raw fields (T4)", "[t4][physpreset][system]")
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
        REQUIRE(infoString.GetValueForKey("sndAliasPrefix") == "physics_metal");
        REQUIRE(infoString.GetValueForKey("piecesSpreadFraction") == "0.125");
        REQUIRE(infoString.GetValueForKey("piecesUpwardVelocity") == "64");
        REQUIRE(infoString.GetValueForKey("canFloat") == "1");
        REQUIRE(infoString.GetValueForKey("gravityScale") == "2");

        auto foundObsoleteField = false;
        infoString.GetValueForKey("tempDefaultToCylinder", &foundObsoleteField);
        REQUIRE_FALSE(foundObsoleteField);
    }
} // namespace
