
#include "Game/IW5/PhysPreset/PhysPresetInfoStringDumperIW5.h"

#include "Game/IW5/ObjConstantsIW5.h"
#include "InfoString/InfoString.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>

using namespace IW5;

namespace
{
    std::unique_ptr<PhysPreset> CreateTestPhysPreset()
    {
        auto physPreset = std::make_unique<PhysPreset>();

        physPreset->name = "test_phys_preset";
        physPreset->mass = 1.25f;
        physPreset->bounce = 0.25f;
        physPreset->friction = std::numeric_limits<float>::max();
        physPreset->bulletForceScale = 0.5f;
        physPreset->explosiveForceScale = 0.75f;
        physPreset->sndAliasPrefix = "physics_metal";
        physPreset->piecesSpreadFraction = 0.125f;
        physPreset->piecesUpwardVelocity = 64.0f;
        physPreset->minMomentum = 1.5f;
        physPreset->maxMomentum = 2.5f;
        physPreset->minPitch = 3.5f;
        physPreset->maxPitch = 4.5f;
        physPreset->volumeType = PHYSPRESET_SCALING_LINEAR;
        physPreset->pitchType = PHYSPRESET_SCALING_QUADRATIC;
        physPreset->tempDefaultToCylinder = true;
        physPreset->perSurfaceSndAlias = true;

        return physPreset;
    }

    std::string DumpPhysPreset(PhysPreset& physPreset)
    {
        Zone dumpingZone("DumpingZone", 0, GameId::IW5, GamePlatform::PC);
        dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<PhysPreset>>(ASSET_TYPE_PHYSPRESET, physPreset.name, &physPreset));

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        phys_preset::InfoStringDumperIW5 dumper;
        dumper.Dump(dumpingContext);

        const auto* dumpedFile = dumpingOutput.GetMockedFile("physic/test_phys_preset");
        REQUIRE(dumpedFile != nullptr);
        return dumpedFile->AsString();
    }

    TEST_CASE("PhysPreset dumper serializes raw fields (IW5)", "[iw5][physpreset][system]")
    {
        const auto testPhysPreset = CreateTestPhysPreset();
        std::istringstream dumpedStream(DumpPhysPreset(*testPhysPreset));

        InfoString infoString;
        REQUIRE(infoString.FromStream(INFO_STRING_PREFIX_PHYS_PRESET, dumpedStream));
        REQUIRE(infoString.GetValueForKey("mass") == "1.25");
        REQUIRE(infoString.GetValueForKey("bounce") == "0.25");
        REQUIRE(infoString.GetValueForKey("friction") == "0");
        REQUIRE(infoString.GetValueForKey("isFrictionInfinity") == "1");
        REQUIRE(infoString.GetValueForKey("bulletForceScale") == "0.5");
        REQUIRE(infoString.GetValueForKey("explosiveForceScale") == "0.75");
        REQUIRE(infoString.GetValueForKey("sndAliasPrefix") == "physics_metal");
        REQUIRE(infoString.GetValueForKey("piecesSpreadFraction") == "0.125");
        REQUIRE(infoString.GetValueForKey("piecesUpwardVelocity") == "64");
        REQUIRE(infoString.GetValueForKey("minMomentum") == "1.5");
        REQUIRE(infoString.GetValueForKey("maxMomentum") == "2.5");
        REQUIRE(infoString.GetValueForKey("minPitch") == "3.5");
        REQUIRE(infoString.GetValueForKey("maxPitch") == "4.5");
        REQUIRE(infoString.GetValueForKey("volumeType") == "linear");
        REQUIRE(infoString.GetValueForKey("pitchType") == "quadratic");
        REQUIRE(infoString.GetValueForKey("tempDefaultToCylinder") == "1");
        REQUIRE(infoString.GetValueForKey("perSurfaceSndAlias") == "1");
    }
} // namespace
