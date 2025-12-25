#include "Game/IW5/GameAssetPoolIW5.h"
#include "Linker.h"
#include "OatTestPaths.h"
#include "SystemTestsPaths.h"
#include "ZoneLoading.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <format>
#include <memory>
#include <string>

namespace fs = std::filesystem;
using namespace std::literals;

namespace
{
    TEST_CASE("Simple Zone(IW5)", "[iw5][system][simple]")
    {
        const auto assetSearchPath = (oat::paths::GetSystemTestsDirectory() / "Game/IW5/Simple").string();
        const auto sourceSearchPath = (oat::paths::GetSystemTestsDirectory() / "Game/IW5/Simple").string();
        const auto outputPath = oat::paths::GetTempDirectory("SimpleZoneIW5").string();

        const char* argStrings[]{
            "SystemTests", // bin
            "--verbose",
            "--asset-search-path",
            assetSearchPath.c_str(),
            "--source-search-path",
            sourceSearchPath.c_str(),
            "--output-folder",
            outputPath.c_str(),
            "SimpleZoneIW5",
        };

        LinkerArgs args;

        bool shouldContinue = true;
        const auto couldParseArgs = args.ParseArgs(std::extent_v<decltype(argStrings)>, argStrings, shouldContinue);

        REQUIRE(couldParseArgs);
        REQUIRE(shouldContinue);

        const auto linker = Linker::Create(std::move(args));
        const auto linkerResult = linker->Start();

        REQUIRE(linkerResult);

        // x64 for now produces invalid zones, don't try to load them yet
#ifdef ARCH_x86
        const auto expectedZonePath = (fs::path(outputPath) / "SimpleZoneIW5.ff").string();
        auto maybeZone = ZoneLoading::LoadZone(expectedZonePath, std::nullopt);
        REQUIRE(maybeZone);

        auto zone = std::move(*maybeZone);
        auto pools = dynamic_cast<GameAssetPoolIW5*>(zone->m_pools.get());

        REQUIRE(zone->m_game_id == GameId::IW5);
        REQUIRE(zone->m_platform == GamePlatform::PC);
        REQUIRE(zone->m_name == "SimpleZoneIW5");
        REQUIRE(pools->GetTotalAssetCount() == 1);
        REQUIRE(pools->m_raw_file->GetAsset("SimpleZone.txt"));
#endif
    }
} // namespace
