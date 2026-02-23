#include "Game/IW5/CommonIW5.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("IW5: Check checksums", "[iw5]")
{
    SECTION("for StringTable_HashString")
    {
        const auto [str, expectedHash] = GENERATE(Catch::Generators::table<const char*, int>({
            {"hello world",                      0x6aefe2c4},
            {"universe2",                        0xe796fe8d},
            {"lit_r0c0n0x0_b1c1n1s1v1_b2c2n2x2", 0x16c4d3f1},
            {"AngularVelocityScale",             0x573e9feb},
            {"BakedLightingIntensity",           0x15af785c},
            {"Layer1OffsetBobbleDelay",          0xfb83324a},
            {"MaxDepth",                         0x1892e3df},
            {"MomentumColor",                    0x5e7abdcb},
            {"SparkleScale",                     0x9e4409f4},
            {"TickMarkColorAndHarshness",        0x5dfe4c47},
            {"worldViewProjectionMatrix",        0xa2501a07},
        }));

        CAPTURE(str);
        const auto hash = IW5::Common::StringTable_HashString(str);
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
        const auto hash = IW5::Common::R_HashString(str);
        REQUIRE(hash == expectedHash);
    }
}
