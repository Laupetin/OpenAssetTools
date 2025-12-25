/*
This makes sure that when reusing assets from an existing zone, asset dependencies that are references can be overwritten to be non-references.

In this case:

- The zone `ZoneWithTechsetT6` contains the techniqueset `trivial_floatz_2992w610`
- The zone `ZoneWithMaterialT6` contains the material `test` and with a reference to its techniqueset `,trivial_floatz_2992w610`
- The final zone `CombinedZoneT6` is built while loading both of the other fastfile and is expected to contain both the material `test` with a reference to a
(actual asset, not a reference) techniqueset `trivial_floatz_2992w610`
*/

#include "Game/T6/GameAssetPoolT6.h"
#include "Linker.h"
#include "OatTestPaths.h"
#include "SystemTestsPaths.h"
#include "Unlinker.h"
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
    void BuildBaseZone(const fs::path& testDir, const fs::path& outputPath)
    {
        const auto testDirStr = testDir.string();
        const auto assetDirStr = (testDir / "BaseZone").string();
        const auto outputPathStr = outputPath.string();

        const char* argStrings[]{
            "SystemTests", // bin
            "--verbose",
            "--add-asset-search-path",
            assetDirStr.c_str(),
            "--source-search-path",
            testDirStr.c_str(),
            "--output-folder",
            outputPathStr.c_str(),
            "BaseZone",
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

    void BuildTestZone(const fs::path& testDir, const fs::path& outputPath)
    {
        const auto testDirStr = testDir.string();
        const auto assetDirStr = (testDir / "TestZone").string();
        const auto outputPathStr = outputPath.string();

        const auto baseZonePathStr = (outputPath / "BaseZone.ff").string();

        const char* argStrings[]{
            "SystemTests", // bin
            "--verbose",
            "--load",
            baseZonePathStr.c_str(),
            "--add-asset-search-path",
            assetDirStr.c_str(),
            "--source-search-path",
            testDirStr.c_str(),
            "--output-folder",
            outputPathStr.c_str(),
            "TestZone",
            "TestZone", // build twice to ensure the second build uses a global asset that the first build already used
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

    void CheckTestZoneContent(const fs::path& outputPath)
    {
        const auto expectedZonePathStr = (outputPath / "TestZone.ff").string();
        auto maybeZone = ZoneLoading::LoadZone(expectedZonePathStr, std::nullopt);
        REQUIRE(maybeZone);

        auto zone = std::move(*maybeZone);
        auto pools = dynamic_cast<GameAssetPoolT6*>(zone->m_pools.get());

        REQUIRE(zone->m_game_id == GameId::T6);
        REQUIRE(zone->m_platform == GamePlatform::PC);
        REQUIRE(zone->m_name == "TestZone");
        REQUIRE(pools->GetTotalAssetCount() == 3);
        REQUIRE(pools->m_material->GetAsset("Suzanne2"));
        REQUIRE(pools->m_technique_set->GetAsset(",trivial_floatz_2992w610"));

        const auto* xmodel = pools->m_xmodel->GetAsset("Suzanne2");
        REQUIRE(xmodel);
        REQUIRE(xmodel->Asset()->boneNames);
        REQUIRE(xmodel->Asset()->numRootBones == 1);
        REQUIRE(xmodel->Asset()->numBones == 3);
        REQUIRE(zone->m_script_strings.Value(xmodel->Asset()->boneNames[0]) == "Root2");
        REQUIRE(zone->m_script_strings.Value(xmodel->Asset()->boneNames[1]) == "EarLeft2");
        REQUIRE(zone->m_script_strings.Value(xmodel->Asset()->boneNames[2]) == "EarRight2");
    }

    // x64 for now produces invalid zones, don't try to load them yet
#ifdef ARCH_x86
    TEST_CASE("Reuse assets from global asset pool(T6)", "[t6][system][simple]")
    {
        const auto testDir = oat::paths::GetSystemTestsDirectory() / "Game/T6/ReuseGlobalAssetPoolsAssets";
        const auto outputPath = oat::paths::GetTempDirectory("ReuseGlobalAssetPoolsAssetsT6");

        BuildBaseZone(testDir, outputPath);
        BuildTestZone(testDir, outputPath);
        CheckTestZoneContent(outputPath);
    }
#endif
} // namespace
