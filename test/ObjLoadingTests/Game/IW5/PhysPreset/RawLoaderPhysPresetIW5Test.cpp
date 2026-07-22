#include "Game/IW5/PhysPreset/RawLoaderPhysPresetIW5.h"

#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace IW5;
using namespace Catch;

namespace
{
    TEST_CASE("PhysPreset loader restores stock raw fields (IW5)", "[iw5][physpreset][system]")
    {
        constexpr auto RAW_PHYS_PRESET = R"(PHYSIC\mass\1.25\bounce\0.25\friction\0.5\isFrictionInfinity\1)"
                                         R"(\bulletForceScale\0.5\explosiveForceScale\0.75)"
                                         R"(\sndAliasPrefix\physics_metal)"
                                         R"(\piecesSpreadFraction\0.125\piecesUpwardVelocity\64)"
                                         R"(\minMomentum\1.0\maxMomentum\2.0)"
                                         R"(\minPitch\3.0\maxPitch\4.0)"
                                         R"(\volumeType\linear\pitchType\quadratic)"
                                         R"(\tempDefaultToCylinder\1\perSurfaceSndAlias\1)";

        MockSearchPath loadingSearchPath;
        loadingSearchPath.AddFileData("physic/test_phys_preset", RAW_PHYS_PRESET);

        Zone loadingZone("LoadingZone", 0, GameId::IW5, GamePlatform::PC);
        AssetCreatorCollection creatorCollection(loadingZone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext loadingContext(loadingZone, &creatorCollection, &ignoredAssetLookup);
        const auto loader = phys_preset::CreateRawLoaderIW5(loadingZone.Memory(), loadingSearchPath, loadingZone);
        const auto result = loader->CreateAsset("test_phys_preset", loadingContext);

        REQUIRE(result.HasBeenSuccessful());
        const auto* loadedAssetInfo = reinterpret_cast<XAssetInfo<PhysPreset>*>(result.GetAssetInfo());
        const auto* loadedPhysPreset = loadedAssetInfo->Asset();

        REQUIRE(std::string(loadedPhysPreset->name) == "test_phys_preset");
        REQUIRE(loadedPhysPreset->mass == Approx(1.25f));
        REQUIRE(loadedPhysPreset->bounce == Approx(0.25f));
        REQUIRE(loadedPhysPreset->friction == Approx(std::numeric_limits<float>::max()));
        REQUIRE(loadedPhysPreset->bulletForceScale == Approx(0.5f));
        REQUIRE(loadedPhysPreset->explosiveForceScale == Approx(0.75f));
        REQUIRE(std::string(loadedPhysPreset->sndAliasPrefix) == "physics_metal");
        REQUIRE(loadedPhysPreset->piecesSpreadFraction == Approx(0.125f));
        REQUIRE(loadedPhysPreset->piecesUpwardVelocity == Approx(64.0f));
        REQUIRE(loadedPhysPreset->minMomentum == Approx(1.0f));
        REQUIRE(loadedPhysPreset->maxMomentum == Approx(2.0f));
        REQUIRE(loadedPhysPreset->minPitch == Approx(3.0f));
        REQUIRE(loadedPhysPreset->maxPitch == Approx(4.0f));
        REQUIRE(loadedPhysPreset->volumeType == PHYSPRESET_SCALING_LINEAR);
        REQUIRE(loadedPhysPreset->pitchType == PHYSPRESET_SCALING_QUADRATIC);
        REQUIRE(loadedPhysPreset->tempDefaultToCylinder == true);
        REQUIRE(loadedPhysPreset->perSurfaceSndAlias == true);
    }
} // namespace
