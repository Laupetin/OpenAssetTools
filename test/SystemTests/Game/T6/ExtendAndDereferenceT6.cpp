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
    void BuildZoneWithMaterial(const fs::path& testDir, const fs::path& outputPath)
    {
        const auto testDirStr = testDir.string();
        const auto outputPathStr = outputPath.string();

        const char* argStrings[]{
            "SystemTests", // bin
            "--verbose",
            "--asset-search-path",
            testDirStr.c_str(),
            "--source-search-path",
            testDirStr.c_str(),
            "--output-folder",
            outputPathStr.c_str(),
            "ZoneWithMaterialT6",
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

    void BuildCombinedZone(const fs::path& testDir, const fs::path& outputPath)
    {
        const auto testDirStr = testDir.string();
        const auto outputPathStr = outputPath.string();

        const auto zoneWithTechsetPathStr = (testDir / "ZoneWithTechsetT6.ff").string();
        const auto zoneWithMaterialPathStr = (outputPath / "ZoneWithMaterialT6.ff").string();

        const char* argStrings[]{
            "SystemTests", // bin
            "--verbose",
            "--load",
            zoneWithTechsetPathStr.c_str(),
            "--load",
            zoneWithMaterialPathStr.c_str(),
            "--source-search-path",
            testDirStr.c_str(),
            "--output-folder",
            outputPathStr.c_str(),
            "CombinedZoneT6",
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

    void CheckCombinedZoneContent(const fs::path& outputPath)
    {
        const auto expectedZonePath = (fs::path(outputPath) / "CombinedZoneT6.ff").string();
        auto maybeZone = ZoneLoading::LoadZone(expectedZonePath, std::nullopt);
        REQUIRE(maybeZone);

        auto zone = std::move(*maybeZone);
        auto pools = dynamic_cast<GameAssetPoolT6*>(zone->m_pools.get());

        REQUIRE(zone->m_game_id == GameId::T6);
        REQUIRE(zone->m_platform == GamePlatform::PC);
        REQUIRE(zone->m_name == "CombinedZoneT6");
        REQUIRE(pools->GetTotalAssetCount() == 2);
        REQUIRE(pools->m_technique_set->GetAsset("trivial_floatz_2992w610"));

        const auto* material = pools->m_material->GetAsset("test");
        REQUIRE(material);
        REQUIRE(material->Asset()->techniqueSet);
        REQUIRE(material->Asset()->techniqueSet->name == "trivial_floatz_2992w610"s);
        REQUIRE(material->Asset()->techniqueSet->techniques[T6::TECHNIQUE_UNLIT]);
    }

    // x64 for now produces invalid zones, don't try to load them yet
#ifdef ARCH_x86
    TEST_CASE("Extend and dereference(T6)", "[t6][system][simple]")
    {
        const auto testDir = oat::paths::GetSystemTestsDirectory() / "Game/T6/ExtendAndDereference";
        const auto outputPath = oat::paths::GetTempDirectory("ExtendAndDereferenceT6");

        BuildZoneWithMaterial(testDir, outputPath);
        BuildCombinedZone(testDir, outputPath);
        CheckCombinedZoneContent(outputPath);
    }
#endif
} // namespace
