#include "Utils/Djb2.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("Djb2: Check checksums", "[djb2]")
{
    SECTION("for djb2")
    {
        const auto [str, expectedHash] = GENERATE(Catch::Generators::table<const char*, uint32_t>({
            {"hello world",                      0x3551c8c1},
            {"universe2",                        0x608d72a8},
            {"lit_r0c0n0x0_b1c1n1s1v1_b2c2n2x2", 0xf93c46aa},
            {"AngularVelocityScale",             0xbba68366},
            {"BakedLightingIntensity",           0x98a9e159},
            {"Layer1OffsetBobbleDelay",          0x319c46af},
            {"MaxDepth",                         0xd34432a0},
            {"MomentumColor",                    0x33a353d6},
            {"SparkleScale",                     0x2249309f},
            {"TickMarkColorAndHarshness",        0x0d77a53c},
            {"worldViewProjectionMatrix",        0xcf668f9a},
        }));

        CAPTURE(str);
        const auto hash = djb2(str);
        REQUIRE(hash == expectedHash);
    }

    SECTION("for djb2_nocase")
    {
        const auto [str, expectedHash] = GENERATE(Catch::Generators::table<const char*, uint32_t>({
            {"hello world",                      0x3551c8c1},
            {"universe2",                        0x608d72a8},
            {"lit_r0c0n0x0_b1c1n1s1v1_b2c2n2x2", 0xd12da30a},
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
        const auto hash = djb2_nocase(str);
        REQUIRE(hash == expectedHash);
    }

    SECTION("for djb2_lower")
    {
        const auto [str, expectedHash] = GENERATE(Catch::Generators::table<const char*, uint32_t>({
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
        const auto hash = djb2_lower(str);
        REQUIRE(hash == expectedHash);
    }

    SECTION("for djb2_xor")
    {
        const auto [str, expectedHash] = GENERATE(Catch::Generators::table<const char*, uint32_t>({
            {"hello world",                      0xf8c65345},
            {"universe2",                        0xdce33172},
            {"lit_r0c0n0x0_b1c1n1s1v1_b2c2n2x2", 0x7e4ed3e0},
            {"AngularVelocityScale",             0xf65bbb88},
            {"BakedLightingIntensity",           0xd79467bd},
            {"Layer1OffsetBobbleDelay",          0xf033afcb},
            {"MaxDepth",                         0x190e08dc},
            {"MomentumColor",                    0xf4ded370},
            {"SparkleScale",                     0x9358a5ef},
            {"TickMarkColorAndHarshness",        0x12d2e5f8},
            {"worldViewProjectionMatrix",        0x88669e4c},
        }));

        CAPTURE(str);
        const auto hash = djb2_xor(str);
        REQUIRE(hash == expectedHash);
    }

    SECTION("for djb2_xor_nocase")
    {
        const auto [str, expectedHash] = GENERATE(Catch::Generators::table<const char*, uint32_t>({
            {"hello world",                      0xf8c65345},
            {"universe2",                        0xdce33172},
            {"lit_r0c0n0x0_b1c1n1s1v1_b2c2n2x2", 0x3cc06840},
            {"AngularVelocityScale",             0x7efc36a8},
            {"BakedLightingIntensity",           0x3d52c39d},
            {"Layer1OffsetBobbleDelay",          0x48574d8b},
            {"MaxDepth",                         0x3c5b84dc},
            {"MomentumColor",                    0x4d32f230},
            {"SparkleScale",                     0x7656c02f},
            {"TickMarkColorAndHarshness",        0x917d8218},
            {"worldViewProjectionMatrix",        0x721c412c},
        }));

        CAPTURE(str);
        const auto hash = djb2_xor_nocase(str);
        REQUIRE(hash == expectedHash);
    }

    SECTION("for djb2_xor_lower")
    {
        const auto [str, expectedHash] = GENERATE(Catch::Generators::table<const char*, uint32_t>({
            {"hello world",                      0xf8c65345},
            {"universe2",                        0xdce33172},
            {"lit_r0c0n0x0_b1c1n1s1v1_b2c2n2x2", 0x7e4ed3e0},
            {"AngularVelocityScale",             0x7efc36a8},
            {"BakedLightingIntensity",           0x3d52c39d},
            {"Layer1OffsetBobbleDelay",          0x48574d8b},
            {"MaxDepth",                         0x3c5b84dc},
            {"MomentumColor",                    0x4d32f230},
            {"SparkleScale",                     0x7656c02f},
            {"TickMarkColorAndHarshness",        0x917d8218},
            {"worldViewProjectionMatrix",        0x721c412c},
        }));

        CAPTURE(str);
        const auto hash = djb2_xor_lower(str);
        REQUIRE(hash == expectedHash);
    }
}
