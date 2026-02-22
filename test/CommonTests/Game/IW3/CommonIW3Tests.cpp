#include "Game/IW3/CommonIW3.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("IW3: Check checksums", "[iw3]")
{
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
        const auto hash = IW3::Common::R_HashString(str);
        REQUIRE(hash == expectedHash);
    }
}
