#include "Game/T6/Techset/TechsetCompilerT6.h"

#include "Game/T6/T6.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/TestMemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <memory>

using namespace T6;
using namespace std::string_literals;

TEST_CASE("Game::T6::Techset::TechsetCompilerT6", "[techset][t6]")
{
    Zone zone("test", 0, GameId::T6, GamePlatform::PC);
    AssetCreatorCollection creators(zone);
    IgnoredAssetLookup ignoredAssets;
    AssetCreationContext context(zone, &creators, &ignoredAssets);
    MockSearchPath searchPath;
    TestMemoryManager memory;
    const auto sut = ::techset::CreateCompilerT6(memory, searchPath);

    SECTION("Sets correct worldVertFormat")
    {
        const auto [techsetName, expectedWorldVertFormat] = GENERATE(Catch::Generators::table<const char*, MaterialWorldVertexFormat>({
            {"default",                                     MTL_WORLDVERT_TEX_1_NRM_1},
            {"effect_zeqqz943",                             MTL_WORLDVERT_TEX_1_NRM_1},
            {"lit_r0c0_t1c1n1",                             MTL_WORLDVERT_TEX_2_NRM_1},
            {"lit_r0c0n0x0_b1c1n1s1v1",                     MTL_WORLDVERT_TEX_2_NRM_2},
            {"lit_r0c0n0x0_b1c1n1s1v1_b2c2n2x2",            MTL_WORLDVERT_TEX_3_NRM_3},
            {"lit_sm_b0c0_b1c1_b2c2",                       MTL_WORLDVERT_TEX_3_NRM_1},
            {"lit_sm_b0c0_b1c1n1x1_b2c2n2v2",               MTL_WORLDVERT_TEX_3_NRM_2},
            {"lit_sm_r0c0_b1c1_b2c2_b3c3",                  MTL_WORLDVERT_TEX_4_NRM_1},
            {"lit_sm_r0c0n0_b1c1_b2c2n2v2_m3c3",            MTL_WORLDVERT_TEX_4_NRM_2},
            {"lit_sm_r0c0n0_b1c1n1s1",                      MTL_WORLDVERT_TEX_2_NRM_2},
            {"lit_sm_r0c0n0s0_b1c1n1s1_b2c2n2",             MTL_WORLDVERT_TEX_3_NRM_3},
            {"lit_sm_r0c0n0x0_b1c1_b2c2n2s2",               MTL_WORLDVERT_TEX_3_NRM_2},
            {"lit_sm_r0c0n0x0_b1c1n1_b2c2n2s2v2",           MTL_WORLDVERT_TEX_3_NRM_3},
            {"lit_sm_r0c0n0x0_b1c1s1v1_m2c2_m3c3",          MTL_WORLDVERT_TEX_4_NRM_1},
            {"lit_sm_r0c0n0x0_b1c1v1_b2c2n2s2_m3c3",        MTL_WORLDVERT_TEX_4_NRM_2},
            {"lit_sm_r0c0s0_b1c1n1s1_m2c2",                 MTL_WORLDVERT_TEX_3_NRM_1},
            {"lit_sm_r0c0x0_b1c1",                          MTL_WORLDVERT_TEX_2_NRM_1},
            {"lit_sm_r0c0x0_b1c1n1s1_b2c2n2s2",             MTL_WORLDVERT_TEX_3_NRM_2},
            {"lit_sm_r0c0x0_m1c1_m2c2_m3c3",                MTL_WORLDVERT_TEX_4_NRM_1},
            {"lit_sm_t0c0n0_b1c1n1v1",                      MTL_WORLDVERT_TEX_2_NRM_2},
            {"lit_sm_t0c0n0s0_b1c1n1_b2c2n2s2v2",           MTL_WORLDVERT_TEX_3_NRM_3},
            {"mc_lit_sm_r0c0d0_2213939z",                   MTL_WORLDVERT_TEX_1_NRM_1},
            {"wpc_lit_sm_b0c0s0_3f3q946z",                  MTL_WORLDVERT_TEX_1_NRM_1},
            {"wpc_lit_sm_r0c0n0s0o0_qj92q1f8",              MTL_WORLDVERT_TEX_1_NRM_1},
            {"wpc_sw4_3d_burning_embers_nuketown_74j6971w", MTL_WORLDVERT_TEX_1_NRM_1},
            {"wpc_unlitdecalblend_add_j26wq580",            MTL_WORLDVERT_TEX_1_NRM_1},
        }));

        CAPTURE(techsetName);
        searchPath.AddFileData(std::format("techsets/{}.techset", techsetName), "");

        const auto result = sut->CreateAsset(techsetName, context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* techset = static_cast<MaterialTechniqueSet*>(result.GetAssetInfo()->m_ptr);
        CHECK(techset->worldVertFormat == expectedWorldVertFormat);
    }
}
