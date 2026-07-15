#include "Game/T6/PhysPreset/RawLoaderPhysPresetT6.h"

#include "Game/T6/GameT6.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <limits>
#include <string>

using namespace T6;
using namespace Catch;
using namespace std::literals;

namespace
{
    TEST_CASE("RawLoaderPhysPreset(T6): Returns no action when file is missing", "[t6][physpreset][assetloader]")
    {
        MockSearchPath searchPath;
        Zone zone("MockZone", 0, GameId::T6, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = phys_preset::CreateRawLoaderT6(memory, searchPath, zone);
        REQUIRE_FALSE(loader->CreateAsset("brick", context).HasBeenSuccessful());
    }

    TEST_CASE("RawLoaderPhysPreset(T6): Can parse physpreset", "[t6][physpreset][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData(
            "physic/brick",
            R"(PHYSIC\mass\20\bounce\0.01\friction\0.3\isFrictionInfinity\0\bulletForceScale\0.6\explosiveForceScale\0.25\piecesSpreadFraction\0.1\piecesUpwardVelocity\0.2\canFloat\1\gravityScale\0.8\massOffsetX\1\massOffsetY\2\massOffsetZ\3\buoyancyMinX\-10\buoyancyMinY\-20\buoyancyMinZ\-30\buoyancyMaxX\10\buoyancyMaxY\20\buoyancyMaxZ\30)");

        Zone zone("MockZone", 0, GameId::T6, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = phys_preset::CreateRawLoaderT6(memory, searchPath, zone);
        auto result = loader->CreateAsset("brick", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* preset = reinterpret_cast<XAssetInfo<PhysPreset>*>(result.GetAssetInfo())->Asset();

        REQUIRE(preset->name == "brick"s);
        REQUIRE(preset->mass == Approx(20.0f * 0.001f));
        REQUIRE(preset->bounce == Approx(0.01f));
        REQUIRE(preset->friction == Approx(0.3f));
        REQUIRE(preset->bulletForceScale == Approx(0.6f));
        REQUIRE(preset->explosiveForceScale == Approx(0.25f));
        REQUIRE(preset->piecesSpreadFraction == Approx(0.1f));
        REQUIRE(preset->piecesUpwardVelocity == Approx(0.2f));
        REQUIRE(preset->canFloat == 1);
        REQUIRE(preset->gravityScale == Approx(0.8f));
        REQUIRE(preset->centerOfMassOffset.x == Approx(1.0f));
        REQUIRE(preset->centerOfMassOffset.y == Approx(2.0f));
        REQUIRE(preset->centerOfMassOffset.z == Approx(3.0f));
        REQUIRE(preset->buoyancyBoxMin.x == Approx(-10.0f));
        REQUIRE(preset->buoyancyBoxMin.y == Approx(-20.0f));
        REQUIRE(preset->buoyancyBoxMin.z == Approx(-30.0f));
        REQUIRE(preset->buoyancyBoxMax.x == Approx(10.0f));
        REQUIRE(preset->buoyancyBoxMax.y == Approx(20.0f));
        REQUIRE(preset->buoyancyBoxMax.z == Approx(30.0f));
    }

    TEST_CASE("RawLoaderPhysPreset(T6): Clamps and scales mass", "[t6][physpreset][assetloader]")
    {
        MockSearchPath searchPath;
        Zone zone("MockZone", 0, GameId::T6, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = phys_preset::CreateRawLoaderT6(memory, searchPath, zone);

        SECTION("normal mass is scaled by 0.001")
        {
            searchPath.AddFileData("physic/brick", R"(PHYSIC\mass\500\bounce\0)");
            auto result = loader->CreateAsset("brick", context);
            REQUIRE(result.HasBeenSuccessful());
            REQUIRE(reinterpret_cast<XAssetInfo<PhysPreset>*>(result.GetAssetInfo())->Asset()->mass == Approx(0.5f));
        }

        SECTION("mass below 1 is clamped to 1 before scaling")
        {
            searchPath.AddFileData("physic/brick", R"(PHYSIC\mass\0\bounce\0)");
            auto result = loader->CreateAsset("brick", context);
            REQUIRE(result.HasBeenSuccessful());
            REQUIRE(reinterpret_cast<XAssetInfo<PhysPreset>*>(result.GetAssetInfo())->Asset()->mass == Approx(0.001f));
        }

        SECTION("mass above 2000 is clamped to 2000 before scaling")
        {
            searchPath.AddFileData("physic/brick", R"(PHYSIC\mass\9999\bounce\0)");
            auto result = loader->CreateAsset("brick", context);
            REQUIRE(result.HasBeenSuccessful());
            REQUIRE(reinterpret_cast<XAssetInfo<PhysPreset>*>(result.GetAssetInfo())->Asset()->mass == Approx(2.0f));
        }
    }

    TEST_CASE("RawLoaderPhysPreset(T6): Sets friction to infinity when isFrictionInfinity is set", "[t6][physpreset][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("physic/brick", R"(PHYSIC\mass\20\bounce\0.01\friction\0.3\isFrictionInfinity\1)");

        Zone zone("MockZone", 0, GameId::T6, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = phys_preset::CreateRawLoaderT6(memory, searchPath, zone);
        auto result = loader->CreateAsset("brick", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* preset = reinterpret_cast<XAssetInfo<PhysPreset>*>(result.GetAssetInfo())->Asset();
        REQUIRE(std::isinf(preset->friction));
    }
} // namespace
