#include "Game/IW4/Techset/VertexDeclCompilerIW4.h"

#include "Game/IW4/IW4.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace IW4;
using namespace Catch;
using namespace std::literals;

TEST_CASE("VertexDeclCompilerIW4", "[iw4][techset][compiler]")
{
    Zone zone("MockZone", 0, GameId::IW4, GamePlatform::PC);
    zone.Register();

    MemoryManager memory;
    AssetCreatorCollection creatorCollection(zone);
    IgnoredAssetLookup ignoredAssetLookup;
    AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

    auto loader = techset::CreateVertexDeclCompilerIW4(memory);

    SECTION("Can create simple vertex decl")
    {
        auto result = loader->CreateAsset("pp", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<MaterialVertexDeclaration>*>(result.GetAssetInfo());
        const auto* decl = assetInfo->Asset();

        CHECK(decl->name == "pp"s);
        CHECK(decl->hasOptionalSource == false);

        REQUIRE(decl->streamCount == 1);
        CHECK(decl->routing.data[0].source == STREAM_SRC_POSITION);
        CHECK(decl->routing.data[0].dest == STREAM_DST_POSITION);
    }

    SECTION("Can create advanced vertex decl")
    {
        auto result = loader->CreateAsset("pdcc1tt7t1t1n1n", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<MaterialVertexDeclaration>*>(result.GetAssetInfo());
        const auto* decl = assetInfo->Asset();

        CHECK(decl->name == "pdcc1tt7t1t1n1n"s);
        CHECK(decl->hasOptionalSource == true);

        REQUIRE(decl->streamCount == 5);
        CHECK(decl->routing.data[0].source == STREAM_SRC_POSITION);
        CHECK(decl->routing.data[0].dest == STREAM_DST_DEPTH);
        CHECK(decl->routing.data[1].source == STREAM_SRC_COLOR);
        CHECK(decl->routing.data[1].dest == STREAM_DST_COLOR_1);
        CHECK(decl->routing.data[2].source == STREAM_SRC_TANGENT);
        CHECK(decl->routing.data[2].dest == STREAM_DST_TEXCOORD_7);
        CHECK(decl->routing.data[3].source == STREAM_SRC_TEXCOORD_1);
        CHECK(decl->routing.data[3].dest == STREAM_DST_TEXCOORD_1);
        CHECK(decl->routing.data[4].source == STREAM_SRC_NORMAL_TRANSFORM_1);
        CHECK(decl->routing.data[4].dest == STREAM_DST_NORMAL);
    }
}
