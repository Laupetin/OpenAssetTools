#include "Game/IW4/PhysPreset/RawLoaderPhysPresetIW4.h"

#include "Game/IW4/GameIW4.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <limits>
#include <string>

using namespace IW4;
using namespace Catch;
using namespace std::literals;

namespace
{
    TEST_CASE("RawLoaderPhysPreset(IW4): Returns no action when file is missing", "[iw4][physpreset][assetloader]")
    {
        MockSearchPath searchPath;
        Zone zone("MockZone", 0, GameId::IW4, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = phys_preset::CreateRawLoaderIW4(memory, searchPath, zone);
        REQUIRE_FALSE(loader->CreateAsset("brick", context).HasBeenSuccessful());
    }

    TEST_CASE("RawLoaderPhysPreset(IW4): Can parse physpreset", "[iw4][physpreset][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("physic/brick",
                               R"(PHYSIC\mass\5\bounce\0.3\friction\0.5\isFrictionInfinity\0\bulletForceScale\0.6\explosiveForceScale\0.12\sndAliasPrefix\physics_brick\piecesSpreadFraction\0.1\piecesUpwardVelocity\0.2\tempDefaultToCylinder\0\perSurfaceSndAlias\1)");

        Zone zone("MockZone", 0, GameId::IW4, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = phys_preset::CreateRawLoaderIW4(memory, searchPath, zone);
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
        REQUIRE(preset->tempDefaultToCylinder == false);
        REQUIRE(preset->perSurfaceSndAlias == true);
    }

    TEST_CASE("RawLoaderPhysPreset(IW4): Sets friction to float max when isFrictionInfinity is set", "[iw4][physpreset][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("physic/brick",
                               R"(PHYSIC\mass\5\bounce\0.3\friction\0.5\isFrictionInfinity\1)");

        Zone zone("MockZone", 0, GameId::IW4, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = phys_preset::CreateRawLoaderIW4(memory, searchPath, zone);
        auto result = loader->CreateAsset("brick", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* preset = reinterpret_cast<XAssetInfo<PhysPreset>*>(result.GetAssetInfo())->Asset();
        REQUIRE(preset->friction == std::numeric_limits<float>::max());
    }
} // namespace
