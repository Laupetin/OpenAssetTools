#include "Linker.h"
#include "OatTestPaths.h"
#include "SystemTestsPaths.h"

#include <catch2/catch_test_macros.hpp>
#include <format>
#include <memory>
#include <string>

using namespace std::literals;

namespace
{
    TEST_CASE("Simple Zone(IW3)", "[iw3][system][simple]")
    {
        const auto assetSearchPath = (oat::paths::GetSystemTestsDirectory() / "Game/IW3/simple").string();
        const auto sourceSearchPath = (oat::paths::GetSystemTestsDirectory() / "Game/IW3/simple").string();
        const auto outputPath = oat::paths::GetTempDirectory("iw3_simple").string();

        const char* argStrings[]{
            "SystemTests", // bin
            "--verbose",
            "--asset-search-path",
            assetSearchPath.c_str(),
            "--source-search-path",
            sourceSearchPath.c_str(),
            "--output-folder",
            outputPath.c_str(),
            "SimpleZoneIW3",
        };

        LinkerArgs args;

        bool shouldContinue = true;
        const auto couldParseArgs = args.ParseArgs(std::extent_v<decltype(argStrings)>, argStrings, shouldContinue);

        REQUIRE(couldParseArgs);
        REQUIRE(shouldContinue);

        const auto linker = Linker::Create(std::move(args));
        const auto linkerResult = linker->Start();

        REQUIRE(linkerResult);
    }
} // namespace
