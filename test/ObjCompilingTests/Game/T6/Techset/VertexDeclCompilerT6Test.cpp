#include "Game/T6/Techset/VertexDeclCompilerT6.h"

#include "Game/T6/T6.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace T6;
using namespace Catch;
using namespace std::literals;

TEST_CASE("VertexDeclCompilerT6", "[t6][techset][compiler]")
{
    Zone zone("MockZone", 0, GameId::T6, GamePlatform::PC);
    zone.Register();

    MemoryManager memory;
    AssetCreatorCollection creatorCollection(zone);
    IgnoredAssetLookup ignoredAssetLookup;
    AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

    auto loader = techset::CreateVertexDeclCompilerT6(memory);

    SECTION("Can create simple vertex decl")
    {
        auto result = loader->CreateSubAsset("pp", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<MaterialVertexDeclaration>*>(result.GetAssetInfo());
        const auto* decl = assetInfo->Asset();

        CHECK(decl->hasOptionalSource == false);
        CHECK(decl->isLoaded == false);

        REQUIRE(decl->streamCount == 1);
        CHECK(decl->routing.data[0].source == STREAM_SRC_POSITION);
        CHECK(decl->routing.data[0].dest == STREAM_DST_POSITION);
    }

    SECTION("Can create advanced vertex decl")
    {
        auto result = loader->CreateSubAsset("pbcc1tt10t1t1n1n", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<MaterialVertexDeclaration>*>(result.GetAssetInfo());
        const auto* decl = assetInfo->Asset();

        CHECK(decl->hasOptionalSource == true);
        CHECK(decl->isLoaded == false);

        REQUIRE(decl->streamCount == 5);
        CHECK(decl->routing.data[0].source == STREAM_SRC_POSITION);
        CHECK(decl->routing.data[0].dest == STREAM_DST_BLENDWEIGHT);
        CHECK(decl->routing.data[1].source == STREAM_SRC_COLOR);
        CHECK(decl->routing.data[1].dest == STREAM_DST_COLOR_1);
        CHECK(decl->routing.data[2].source == STREAM_SRC_TANGENT);
        CHECK(decl->routing.data[2].dest == STREAM_DST_TEXCOORD_10);
        CHECK(decl->routing.data[3].source == STREAM_SRC_TEXCOORD_1);
        CHECK(decl->routing.data[3].dest == STREAM_DST_TEXCOORD_1);
        CHECK(decl->routing.data[4].source == STREAM_SRC_NORMAL_TRANSFORM_1);
        CHECK(decl->routing.data[4].dest == STREAM_DST_NORMAL);
    }
}
