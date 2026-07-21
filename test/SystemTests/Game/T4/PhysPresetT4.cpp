#include "Game/T4/ObjConstantsT4.h"
#include "Game/T4/PhysPreset/PhysPresetInfoStringDumperT4.h"
#include "Game/T4/PhysPreset/RawLoaderPhysPresetT4.h"
#include "InfoString/InfoString.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"
#include "ZoneLoading.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <sstream>
#include <string>

using namespace T4;

namespace
{
    constexpr auto PHYS_PRESET_NAME = "test_phys_preset";
    constexpr auto SOUND_ALIAS_PREFIX = "physics_metal";

    std::unique_ptr<PhysPreset> CreateTestPhysPreset()
    {
        auto physPreset = std::make_unique<PhysPreset>();
        physPreset->name = PHYS_PRESET_NAME;
        physPreset->mass = 1.25f;
        physPreset->bounce = 0.25f;
        physPreset->friction = MAX_FRICTION;
        physPreset->bulletForceScale = 0.5f;
        physPreset->explosiveForceScale = 0.75f;
        physPreset->sndAliasPrefix = SOUND_ALIAS_PREFIX;
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
        REQUIRE(infoString.GetValueForKey("sndAliasPrefix") == SOUND_ALIAS_PREFIX);
        REQUIRE(infoString.GetValueForKey("piecesSpreadFraction") == "0.125");
        REQUIRE(infoString.GetValueForKey("piecesUpwardVelocity") == "64");
        REQUIRE(infoString.GetValueForKey("canFloat") == "1");
        REQUIRE(infoString.GetValueForKey("gravityScale") == "2");

        auto foundObsoleteField = false;
        infoString.GetValueForKey("tempDefaultToCylinder", &foundObsoleteField);
        REQUIRE_FALSE(foundObsoleteField);
    }

    TEST_CASE("PhysPreset loader restores stock raw fields (T4)", "[t4][physpreset][system]")
    {
        constexpr auto RAW_PHYS_PRESET = R"(PHYSIC\sndAliasPrefix\physics_metal)"
                                         R"(\mass\1250\friction\0.5\isFrictionInfinity\1\bounce\0.25)"
                                         R"(\bulletForceScale\0.5\explosiveForceScale\0.75)"
                                         R"(\piecesSpreadFraction\0.125\piecesUpwardVelocity\64)"
                                         R"(\tempDefaultToCylinder\1\canFloat\1\gravityScale\2)";

        MockSearchPath loadingSearchPath;
        loadingSearchPath.AddFileData("physic/test_phys_preset", RAW_PHYS_PRESET);

        Zone loadingZone("LoadingZone", 0, GameId::T4, GamePlatform::PC);
        AssetCreatorCollection creatorCollection(loadingZone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext loadingContext(loadingZone, &creatorCollection, &ignoredAssetLookup);
        const auto loader = phys_preset::CreateRawLoaderT4(loadingZone.Memory(), loadingSearchPath, loadingZone);
        const auto result = loader->CreateAsset(PHYS_PRESET_NAME, loadingContext);

        REQUIRE(result.HasBeenSuccessful());
        const auto* loadedAssetInfo = reinterpret_cast<XAssetInfo<PhysPreset>*>(result.GetAssetInfo());
        const auto* loadedPhysPreset = loadedAssetInfo->Asset();

        REQUIRE(std::string(loadedPhysPreset->name) == PHYS_PRESET_NAME);
        REQUIRE(loadedPhysPreset->mass == 1.25f);
        REQUIRE(loadedPhysPreset->bounce == 0.25f);
        REQUIRE(loadedPhysPreset->friction == MAX_FRICTION);
        REQUIRE(loadedPhysPreset->bulletForceScale == 0.5f);
        REQUIRE(loadedPhysPreset->explosiveForceScale == 0.75f);
        REQUIRE(std::string(loadedPhysPreset->sndAliasPrefix) == SOUND_ALIAS_PREFIX);
        REQUIRE(loadedPhysPreset->piecesSpreadFraction == 0.125f);
        REQUIRE(loadedPhysPreset->piecesUpwardVelocity == 64.0f);
        REQUIRE(loadedPhysPreset->canFloat == 1);
        REQUIRE(loadedPhysPreset->gravityScale == 2.0f);
    }
} // namespace
