#include "Game/T6/CommonT6.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("T6: Check checksums", "[t6]")
{
    SECTION("for Com_HashKey")
    {
        const auto [str, expectedHash] = GENERATE(Catch::Generators::table<const char*, int>({
            {"hello world",                      0x21c3f},
            {"universe2",                        0x1be55},
            {"lit_r0c0n0x0_b1c1n1s1v1_b2c2n2x2", 0x56e0f},
            {"AngularVelocityScale",             0x4043a},
            {"BakedLightingIntensity",           0x47a5c},
            {"Layer1OffsetBobbleDelay",          0x47950},
            {"MaxDepth",                         0x17ca1},
            {"MomentumColor",                    0x2998e},
            {"SparkleScale",                     0x24cea},
            {"TickMarkColorAndHarshness",        0x50d40},
            {"worldViewProjectionMatrix",        0x548c4},
        }));

        CAPTURE(str);
        const auto hash = T6::Common::Com_HashKey(str, 64);
        REQUIRE(hash == expectedHash);
    }

    SECTION("for Com_HashString")
    {
        const auto [str, expectedHash] = GENERATE(Catch::Generators::table<const char*, int>({
            {"hello world",                      0x3551c8c1},
            {"universe2",                        0x608d72a8},
            {"lit_r0c0n0x0_b1c1n1s1v1_b2c2n2x2", 0xf93c46aa},
            {"AngularVelocityScale",             0x2eb08fc6},
            {"BakedLightingIntensity",           0xd8fd95b9},
            {"Layer1OffsetBobbleDelay",          0x4166172f},
            {"MaxDepth",                         0x5dc1dee0},
            {"MomentumColor",                    0x21c79416},
            {"SparkleScale",                     0x42f7ecdf},
            {"TickMarkColorAndHarshness",        0xbb5761dc},
            {"worldViewProjectionMatrix",        0x25962bfa},
        }));

        CAPTURE(str);
        const auto hash = T6::Common::Com_HashString(str);
        REQUIRE(hash == expectedHash);
    }

    SECTION("for R_HashString")
    {
        const auto [str, expectedHash] = GENERATE(Catch::Generators::table<const char*, uint32_t>({
            {"hello world",                      0xe7d74060},
            {"universe2",                        0x113fdcd7},
            {"lit_r0c0n0x0_b1c1n1s1v1_b2c2n2x2", 0xdd0153c5},
            {"AngularVelocityScale",             0x18f2cb6d},
            {"BakedLightingIntensity",           0xd627f218},
            {"Layer1OffsetBobbleDelay",          0xcd91b6ae},
            {"MaxDepth",                         0x61ed5959},
            {"MomentumColor",                    0xc80f3595},
            {"SparkleScale",                     0x5488816a},
            {"TickMarkColorAndHarshness",        0xd6c718bd},
            {"worldViewProjectionMatrix",        0x7f661409},
        }));

        CAPTURE(str);
        const auto hash = T6::Common::R_HashString(str);
        REQUIRE(hash == expectedHash);
    }

    SECTION("for SND_HashName")
    {
        const auto [str, expectedHash] = GENERATE(Catch::Generators::table<const char*, uint32_t>({
            {"hello world",                      0x9e420d7f},
            {"universe2",                        0xee605328},
            {"lit_r0c0n0x0_b1c1n1s1v1_b2c2n2x2", 0xc38575b6},
            {"AngularVelocityScale",             0x0c032bd0},
            {"BakedLightingIntensity",           0x57f1a221},
            {"Layer1OffsetBobbleDelay",          0x4f640b85},
            {"MaxDepth",                         0x50d1cae4},
            {"MomentumColor",                    0xbcab7666},
            {"SparkleScale",                     0x162259d9},
            {"TickMarkColorAndHarshness",        0xbf3d7a42},
            {"worldViewProjectionMatrix",        0x28db35e2},
        }));

        CAPTURE(str);
        const auto hash = T6::Common::SND_HashName(str);
        REQUIRE(hash == expectedHash);
    }
}
