#include "UnlinkerArgs.h"

#include <catch2/catch_test_macros.hpp>
#include <type_traits>

namespace
{
    TEST_CASE("Unlinker uses the default output folder when none is specified", "[unlinker][arguments]")
    {
        const char* argStrings[]{
            "Unlinker",
            "test.ff",
        };

        UnlinkerArgs args;
        bool shouldContinue = true;

        REQUIRE(args.ParseArgs(std::extent_v<decltype(argStrings)>, argStrings, shouldContinue));
        REQUIRE(shouldContinue);
        REQUIRE(args.m_output_folder == UnlinkerArgs::DEFAULT_OUTPUT_FOLDER);

        const Zone zone("test_zone", 0, GameId::IW4, GamePlatform::PC);
        REQUIRE(args.GetOutputFolderPathForZone(zone) == "zone_dump/iw4/test_zone");
    }

    TEST_CASE("Unlinker output folder remains unchanged without placeholders", "[unlinker][arguments]")
    {
        UnlinkerArgs args;
        args.m_output_folder = "zone_dump/custom";

        const Zone zone("test_zone", 0, GameId::IW4, GamePlatform::PC);

        REQUIRE(args.GetOutputFolderPathForZone(zone) == "zone_dump/custom");
    }

    TEST_CASE("Unlinker output folder replaces game and zone placeholders", "[unlinker][arguments]")
    {
        UnlinkerArgs args;
        args.m_output_folder = "zone_dump/?game?/?game?/?zone?";

        const Zone zone("test_zone", 0, GameId::IW4, GamePlatform::PC);

        REQUIRE(args.GetOutputFolderPathForZone(zone) == "zone_dump/iw4/iw4/test_zone");
    }
} // namespace
