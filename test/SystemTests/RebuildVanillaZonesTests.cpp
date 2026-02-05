#include "Game/T6/T6.h"
#include "OatTestPaths.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"
#include "ZoneLoading.h"
#include "ZoneWriting.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <string>

namespace fs = std::filesystem;
using namespace std::literals;

/*
 * This test is only run when the user defines the environment variable OAT_FASTFILE_DIR containing the path to a folder containing fastfiles.
 */
namespace
{
    void EnsureCanRebuildFastFile(const fs::path& outputPath, const fs::path& fastFilePath)
    {
        const auto fastFilePathStr = fastFilePath.string();
        const auto tempFilePath = outputPath / "temp.ff";
        const auto tempFilePathStr = tempFilePath.string();

        con::info("Testing for rebuild: {}", fastFilePathStr);

        auto maybeZone = ZoneLoading::LoadZone(fastFilePathStr, std::nullopt);
        REQUIRE(maybeZone.has_value());

        auto zone = std::move(*maybeZone);
        zone->m_name = "temp";

        {
            std::ofstream outStream(tempFilePath, std::ios::out | std::ios::binary);
            REQUIRE(outStream.is_open());

            const auto linkingResult = ZoneWriting::WriteZone(outStream, *zone);
            REQUIRE(linkingResult);
        }

        auto maybeRebuiltZone = ZoneLoading::LoadZone(tempFilePathStr, std::nullopt);
        REQUIRE(maybeRebuiltZone.has_value());

        auto rebuiltZone = std::move(*maybeRebuiltZone);

        const auto& pools = zone->m_pools;
        const auto& rebuiltPools = rebuiltZone->m_pools;

        const auto totalAssetCount = pools.GetTotalAssetCount();
        REQUIRE(totalAssetCount == rebuiltPools.GetTotalAssetCount());

        auto zoneIter = pools.begin();
        auto zoneEnd = pools.end();
        auto rebuiltZoneIter = rebuiltPools.begin();
        auto rebuiltZoneEnd = rebuiltPools.end();

        while (zoneIter != zoneEnd)
        {
            REQUIRE(rebuiltZoneIter != rebuiltZoneEnd);

            REQUIRE((*zoneIter)->m_type == (*rebuiltZoneIter)->m_type);
            REQUIRE((*zoneIter)->m_name == (*rebuiltZoneIter)->m_name);

            ++zoneIter;
            ++rebuiltZoneIter;
        }
    }

    TEST_CASE("Rebuild vanilla zones", "[custom]")
    {
        auto* fastFilePathStr = std::getenv("OAT_FASTFILE_DIR");
        if (!fastFilePathStr)
            return;

        const auto outputPath = oat::paths::GetTempDirectory("RebuildVanillaZones");
        const fs::path fastFilePath(fastFilePathStr);

        fs::directory_iterator iterator(fastFilePath);
        const auto end = fs::end(iterator);
        for (auto i = fs::begin(iterator); i != end; ++i)
        {
            if (!i->is_regular_file())
                continue;

            const auto path = i->path();
            auto extension = path.extension().string();
            utils::MakeStringLowerCase(extension);
            if (extension == ".ff")
                EnsureCanRebuildFastFile(outputPath, path);
        }
    }
} // namespace
