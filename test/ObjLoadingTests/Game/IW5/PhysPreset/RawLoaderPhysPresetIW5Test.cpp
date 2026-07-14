#include "Game/IW5/PhysPreset/RawLoaderPhysPresetIW5.h"

#include "Game/IW5/GameIW5.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <limits>
#include <string>

using namespace IW5;
using namespace Catch;
using namespace std::literals;

namespace
{
    TEST_CASE("RawLoaderPhysPreset(IW5): Returns no action when file is missing", "[iw5][physpreset][assetloader]")
    {
        MockSearchPath searchPath;
        Zone zone("MockZone", 0, GameId::IW5, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = phys_preset::CreateRawLoaderIW5(memory, searchPath, zone);
        REQUIRE_FALSE(loader->CreateAsset("brick", context).HasBeenSuccessful());
    }

    TEST_CASE("RawLoaderPhysPreset(IW5): Can parse physpreset", "[iw5][physpreset][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("physic/brick",
                               R"(PHYSIC\mass\5\bounce\0.3\friction\0.5\isFrictionInfinity\0\bulletForceScale\0.6\explosiveForceScale\0.12\sndAliasPrefix\physics_brick\piecesSpreadFraction\0.1\piecesUpwardVelocity\0.2\minMomentum\10\maxMomentum\100\minPitch\0.5\maxPitch\1.5\volumeType\linear\pitchType\quadratic\tempDefaultToCylinder\0\perSurfaceSndAlias\1)");

        Zone zone("MockZone", 0, GameId::IW5, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = phys_preset::CreateRawLoaderIW5(memory, searchPath, zone);
        auto result = loader->CreateAsset("brick", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* preset = reinterpret_cast<XAssetInfo<PhysPreset>*>(result.GetAssetInfo())->Asset();

        REQUIRE(preset->name == "brick"s);
        REQUIRE(preset->mass == Approx(5.0f));
        REQUIRE(preset->bounce == Approx(0.3f));
        REQUIRE(preset->friction == Approx(0.5f));
        REQUIRE(preset->bulletForceScale == Approx(0.6f));
        REQUIRE(preset->explosiveForceScale == Approx(0.12f));
        REQUIRE(preset->sndAliasPrefix == "physics_brick"s);
        REQUIRE(preset->piecesSpreadFraction == Approx(0.1f));
        REQUIRE(preset->piecesUpwardVelocity == Approx(0.2f));
        REQUIRE(preset->minMomentum == Approx(10.0f));
        REQUIRE(preset->maxMomentum == Approx(100.0f));
        REQUIRE(preset->minPitch == Approx(0.5f));
        REQUIRE(preset->maxPitch == Approx(1.5f));
        REQUIRE(preset->volumeType == PHYSPRESET_SCALING_LINEAR);
        REQUIRE(preset->pitchType == PHYSPRESET_SCALING_QUADRATIC);
        REQUIRE(preset->tempDefaultToCylinder == false);
        REQUIRE(preset->perSurfaceSndAlias == true);
    }

    TEST_CASE("RawLoaderPhysPreset(IW5): Sets friction to float max when isFrictionInfinity is set", "[iw5][physpreset][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("physic/brick",
                               R"(PHYSIC\mass\5\bounce\0.3\friction\0.5\isFrictionInfinity\1)");

        Zone zone("MockZone", 0, GameId::IW5, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = phys_preset::CreateRawLoaderIW5(memory, searchPath, zone);
        auto result = loader->CreateAsset("brick", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* preset = reinterpret_cast<XAssetInfo<PhysPreset>*>(result.GetAssetInfo())->Asset();
        REQUIRE(preset->friction == std::numeric_limits<float>::max());
    }
} // namespace
