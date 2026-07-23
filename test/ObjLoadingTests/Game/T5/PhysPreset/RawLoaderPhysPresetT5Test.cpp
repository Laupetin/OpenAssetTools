#include "Game/T5/PhysPreset/RawLoaderPhysPresetT5.h"

#include "Game/T5/ObjConstantsT5.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace T5;
using namespace Catch;

namespace
{
    TEST_CASE("PhysPreset loader restores stock raw fields (T5)", "[t5][physpreset][system]")
    {
        constexpr auto RAW_PHYS_PRESET = R"(PHYSIC\mass\1250\bounce\0.25\friction\0.5\isFrictionInfinity\1)"
                                         R"(\bulletForceScale\0.5\explosiveForceScale\0.75)"
                                         R"(\piecesSpreadFraction\0.125\piecesUpwardVelocity\64)"
                                         R"(\canFloat\1\gravityScale\2)"
                                         R"(\massOffsetX\1\massOffsetY\2\massOffsetZ\3)"
                                         R"(\buoyancyMinX\3\buoyancyMinY\4\buoyancyMinZ\5)"
                                         R"(\buoyancyMaxX\6\buoyancyMaxY\7\buoyancyMaxZ\8)";

        MockSearchPath loadingSearchPath;
        loadingSearchPath.AddFileData("physic/test_phys_preset", RAW_PHYS_PRESET);

        Zone loadingZone("LoadingZone", 0, GameId::T5, GamePlatform::PC);
        AssetCreatorCollection creatorCollection(loadingZone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext loadingContext(loadingZone, &creatorCollection, &ignoredAssetLookup);
        const auto loader = phys_preset::CreateRawLoaderT5(loadingZone.Memory(), loadingSearchPath, loadingZone);
        const auto result = loader->CreateAsset("test_phys_preset", loadingContext);

        REQUIRE(result.HasBeenSuccessful());
        const auto* loadedAssetInfo = reinterpret_cast<XAssetInfo<PhysPreset>*>(result.GetAssetInfo());
        const auto* loadedPhysPreset = loadedAssetInfo->Asset();

        REQUIRE(std::string(loadedPhysPreset->name) == "test_phys_preset");
        REQUIRE(loadedPhysPreset->mass == Approx(1.25f));
        REQUIRE(loadedPhysPreset->bounce == Approx(0.25f));
        REQUIRE(loadedPhysPreset->friction == Approx(PHYS_PRESET_MAX_FRICTION));
        REQUIRE(loadedPhysPreset->bulletForceScale == Approx(0.5f));
        REQUIRE(loadedPhysPreset->explosiveForceScale == Approx(0.75f));
        REQUIRE(loadedPhysPreset->piecesSpreadFraction == Approx(0.125f));
        REQUIRE(loadedPhysPreset->piecesUpwardVelocity == Approx(64.0f));
        REQUIRE(loadedPhysPreset->canFloat == 1);
        REQUIRE(loadedPhysPreset->gravityScale == Approx(2.0f));
        REQUIRE(loadedPhysPreset->centerOfMassOffset.x == Approx(1.0f));
        REQUIRE(loadedPhysPreset->centerOfMassOffset.y == Approx(2.0f));
        REQUIRE(loadedPhysPreset->centerOfMassOffset.z == Approx(3.0f));
        REQUIRE(loadedPhysPreset->buoyancyBoxMin.x == Approx(3.0f));
        REQUIRE(loadedPhysPreset->buoyancyBoxMin.y == Approx(4.0f));
        REQUIRE(loadedPhysPreset->buoyancyBoxMin.z == Approx(5.0f));
        REQUIRE(loadedPhysPreset->buoyancyBoxMax.x == Approx(6.0f));
        REQUIRE(loadedPhysPreset->buoyancyBoxMax.y == Approx(7.0f));
        REQUIRE(loadedPhysPreset->buoyancyBoxMax.z == Approx(8.0f));
    }
} // namespace
