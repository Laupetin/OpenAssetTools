#include "Game/T4/PhysPreset/RawLoaderPhysPresetT4.h"

#include "Game/T4/GameT4.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace T4;
using namespace Catch;
using namespace std::literals;

namespace
{
    TEST_CASE("RawLoaderPhysPreset(T4): Returns no action when file is missing", "[t4][physpreset][assetloader]")
    {
        MockSearchPath searchPath;
        Zone zone("MockZone", 0, GameId::T4, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = phys_preset::CreateRawLoaderT4(memory, searchPath, zone);
        REQUIRE_FALSE(loader->CreateAsset("brick", context).HasBeenSuccessful());
    }

    TEST_CASE("RawLoaderPhysPreset(T4): Can parse physpreset", "[t4][physpreset][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("physic/brick",
                               R"(PHYSIC\mass\20\bounce\0.01\friction\0.3\isFrictionInfinity\0\bulletForceScale\0.6\explosiveForceScale\0.25\sndAliasPrefix\physics_brick\piecesSpreadFraction\0.1\piecesUpwardVelocity\0.2\canFloat\0\gravityScale\1.0)");

        Zone zone("MockZone", 0, GameId::T4, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = phys_preset::CreateRawLoaderT4(memory, searchPath, zone);
        auto result = loader->CreateAsset("brick", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* preset = reinterpret_cast<XAssetInfo<PhysPreset>*>(result.GetAssetInfo())->Asset();

        REQUIRE(preset->name == "brick"s);
        REQUIRE(preset->mass == Approx(20.0f));
        REQUIRE(preset->bounce == Approx(0.01f));
        REQUIRE(preset->friction == Approx(0.3f));
        REQUIRE(preset->bulletForceScale == Approx(0.6f));
        REQUIRE(preset->explosiveForceScale == Approx(0.25f));
        REQUIRE(preset->sndAliasPrefix == "physics_brick"s);
        REQUIRE(preset->piecesSpreadFraction == Approx(0.1f));
        REQUIRE(preset->piecesUpwardVelocity == Approx(0.2f));
        REQUIRE(preset->canFloat == 0);
        REQUIRE(preset->gravityScale == Approx(1.0f));
    }

    TEST_CASE("RawLoaderPhysPreset(T4): Sets friction to MAX_FRICTION when isFrictionInfinity is set", "[t4][physpreset][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("physic/brick",
                               R"(PHYSIC\mass\20\bounce\0.01\friction\0.3\isFrictionInfinity\1)");

        Zone zone("MockZone", 0, GameId::T4, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = phys_preset::CreateRawLoaderT4(memory, searchPath, zone);
        auto result = loader->CreateAsset("brick", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* preset = reinterpret_cast<XAssetInfo<PhysPreset>*>(result.GetAssetInfo())->Asset();
        REQUIRE(preset->friction == Approx(1e+10f));
    }
} // namespace
