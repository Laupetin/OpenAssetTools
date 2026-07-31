#include "Game/T4/PhysPreset/RawLoaderPhysPresetT4.h"

#include "Game/T4/ObjConstantsT4.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace T4;
using namespace Catch;

namespace
{
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
        REQUIRE(std::string(loadedPhysPreset->sndAliasPrefix) == "physics_metal");
        REQUIRE(loadedPhysPreset->piecesSpreadFraction == Approx(0.125f));
        REQUIRE(loadedPhysPreset->piecesUpwardVelocity == Approx(64.0f));
        REQUIRE(loadedPhysPreset->canFloat == 1);
        REQUIRE(loadedPhysPreset->gravityScale == Approx(2.0f));
    }
} // namespace
