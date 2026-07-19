#include "Game/IW4/IW4.h"
#include "Linker.h"
#include "OatTestPaths.h"
#include "SystemTestsPaths.h"
#include "ZoneLoading.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>

namespace fs = std::filesystem;
using namespace std::literals;

namespace
{
    TEST_CASE("Simple Zone(IW4)", "[iw4][system][simple]")
    {
        const auto assetSearchPath = (oat::paths::GetSystemTestsDirectory() / "Game/IW4/Simple").string();
        const auto sourceSearchPath = (oat::paths::GetSystemTestsDirectory() / "Game/IW4/Simple").string();
        const auto outputPath = oat::paths::GetTempDirectory("SimpleZoneIW4").string();

        const char* argStrings[]{
            "SystemTests", // bin
            "--verbose",
            "--no-assetlist",
            "--asset-search-path",
            assetSearchPath.c_str(),
            "--source-search-path",
            sourceSearchPath.c_str(),
            "--output-folder",
            outputPath.c_str(),
            "SimpleZoneIW4",
        };

        LinkerArgs args;

        bool shouldContinue = true;
        const auto couldParseArgs = args.ParseArgs(std::extent_v<decltype(argStrings)>, argStrings, shouldContinue);

        REQUIRE(couldParseArgs);
        REQUIRE(shouldContinue);

        const auto linker = Linker::Create(std::move(args));
        const auto linkerResult = linker->Start();

        REQUIRE(linkerResult);

        const auto expectedZonePath = (fs::path(outputPath) / "SimpleZoneIW4.ff").string();
        auto maybeZone = ZoneLoading::LoadZone(expectedZonePath, std::nullopt);
        REQUIRE(maybeZone);

        auto zone = std::move(*maybeZone);

        REQUIRE(zone->m_game_id == GameId::IW4);
        REQUIRE(zone->m_platform == GamePlatform::PC);
        REQUIRE(zone->m_name == "SimpleZoneIW4");
        REQUIRE(zone->m_pools.GetTotalAssetCount() == 1);
        REQUIRE(zone->m_pools.GetAsset<IW4::AssetRawFile>("SimpleZone.txt"));
    }

    TEST_CASE("Linker generates an assetlist(IW4)", "[iw4][system][assetlist]")
    {
        const auto assetSearchPath = (oat::paths::GetSystemTestsDirectory() / "Game/IW4/Simple").string();
        const auto sourceSearchPath = (oat::paths::GetSystemTestsDirectory() / "Game/IW4/AssetList").string();
        const auto outputPath = oat::paths::GetTempDirectory("SimpleZoneIW4AssetList").string();
        fs::create_directories(fs::path(outputPath) / ".oat/cache/SimpleZoneIW4");

        const char* argStrings[]{
            "SystemTests", // bin
            "--asset-search-path",
            assetSearchPath.c_str(),
            "--source-search-path",
            sourceSearchPath.c_str(),
            "--base-folder",
            outputPath.c_str(),
            "--output-folder",
            outputPath.c_str(),
            "SimpleZoneIW4",
        };

        LinkerArgs args;

        bool shouldContinue = true;
        const auto couldParseArgs = args.ParseArgs(std::extent_v<decltype(argStrings)>, argStrings, shouldContinue);

        REQUIRE(couldParseArgs);
        REQUIRE(shouldContinue);

        const auto linker = Linker::Create(std::move(args));
        REQUIRE(linker->Start());

        const auto expectedAssetListPath = fs::path(outputPath) / "zone_source/assetlist/SimpleZoneIW4.csv";
        std::ifstream assetListStream(expectedAssetListPath, std::ios::binary);
        REQUIRE(assetListStream.is_open());

        const std::string assetListContent(std::istreambuf_iterator<char>(assetListStream), {});
        REQUIRE(assetListContent == "material,,white\nrawfile,SimpleZone.txt\n");
    }

    TEST_CASE("Linker can omit the assetlist(IW4)", "[iw4][system][assetlist]")
    {
        const auto assetSearchPath = (oat::paths::GetSystemTestsDirectory() / "Game/IW4/Simple").string();
        const auto sourceSearchPath = (oat::paths::GetSystemTestsDirectory() / "Game/IW4/Simple").string();
        const auto outputPath = oat::paths::GetTempDirectory("SimpleZoneIW4NoAssetList").string();
        fs::create_directories(fs::path(outputPath) / ".oat/cache/SimpleZoneIW4");

        const char* argStrings[]{
            "SystemTests", // bin
            "--asset-search-path",
            assetSearchPath.c_str(),
            "--source-search-path",
            sourceSearchPath.c_str(),
            "--base-folder",
            outputPath.c_str(),
            "--output-folder",
            outputPath.c_str(),
            "--no-assetlist",
            "SimpleZoneIW4",
        };

        LinkerArgs args;

        bool shouldContinue = true;
        const auto couldParseArgs = args.ParseArgs(std::extent_v<decltype(argStrings)>, argStrings, shouldContinue);

        REQUIRE(couldParseArgs);
        REQUIRE(shouldContinue);

        const auto linker = Linker::Create(std::move(args));
        REQUIRE(linker->Start());

        const auto expectedAssetListPath = fs::path(outputPath) / "zone_source/assetlist/SimpleZoneIW4.csv";
        REQUIRE_FALSE(fs::exists(expectedAssetListPath));
    }

    TEST_CASE("Linker can use a generated assetlist as ignore(IW4)", "[iw4][system][assetlist]")
    {
        const auto assetSearchPath = (oat::paths::GetSystemTestsDirectory() / "Game/IW4/Simple").string();
        const auto assetListSourcePath = (oat::paths::GetSystemTestsDirectory() / "Game/IW4/AssetList").string();
        const auto ignoreSourcePath = (oat::paths::GetSystemTestsDirectory() / "Game/IW4/AssetListIgnore").string();
        const auto outputPath = oat::paths::GetTempDirectory("GeneratedAssetListIgnoreIW4").string();
        fs::create_directories(fs::path(outputPath) / ".oat/cache/SimpleZoneIW4");
        fs::create_directories(fs::path(outputPath) / ".oat/cache/IgnoreAssetListIW4");

        const char* generateArgStrings[]{
            "SystemTests", // bin
            "--asset-search-path",
            assetSearchPath.c_str(),
            "--source-search-path",
            assetListSourcePath.c_str(),
            "--base-folder",
            outputPath.c_str(),
            "--output-folder",
            outputPath.c_str(),
            "SimpleZoneIW4",
        };

        LinkerArgs generateArgs;
        bool shouldContinue = true;
        REQUIRE(generateArgs.ParseArgs(std::extent_v<decltype(generateArgStrings)>, generateArgStrings, shouldContinue));
        REQUIRE(shouldContinue);
        REQUIRE(Linker::Create(std::move(generateArgs))->Start());

        const auto generatedSourcePath = (fs::path(outputPath) / "zone_source").string();
        const char* ignoreArgStrings[]{
            "SystemTests", // bin
            "--asset-search-path",
            assetSearchPath.c_str(),
            "--source-search-path",
            generatedSourcePath.c_str(),
            "--add-source-search-path",
            ignoreSourcePath.c_str(),
            "--base-folder",
            outputPath.c_str(),
            "--output-folder",
            outputPath.c_str(),
            "--no-assetlist",
            "IgnoreAssetListIW4",
        };

        LinkerArgs ignoreArgs;
        shouldContinue = true;
        REQUIRE(ignoreArgs.ParseArgs(std::extent_v<decltype(ignoreArgStrings)>, ignoreArgStrings, shouldContinue));
        REQUIRE(shouldContinue);
        REQUIRE(Linker::Create(std::move(ignoreArgs))->Start());

        const auto expectedZonePath = (fs::path(outputPath) / "IgnoreAssetListIW4.ff").string();
        auto maybeZone = ZoneLoading::LoadZone(expectedZonePath, std::nullopt);
        REQUIRE(maybeZone);

        auto zone = std::move(*maybeZone);
        REQUIRE(zone->m_pools.GetTotalAssetCount() == 3);

        const auto* material = zone->m_pools.GetAssetOrAssetReference<IW4::AssetMaterial>("white");
        REQUIRE(material);
        REQUIRE(material->IsReference());

        const auto* rawFile = zone->m_pools.GetAssetOrAssetReference<IW4::AssetRawFile>("SimpleZone.txt");
        REQUIRE(rawFile);
        REQUIRE(rawFile->IsReference());

        const auto* notIgnoredRawFile = zone->m_pools.GetAsset<IW4::AssetRawFile>("NotIgnored.txt");
        REQUIRE(notIgnoredRawFile);
        REQUIRE_FALSE(notIgnoredRawFile->IsReference());
    }
} // namespace
