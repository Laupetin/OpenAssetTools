#include "ZoneLoading.h"

#include "Loading/IZoneLoaderFactory.h"
#include "Loading/ZoneDataPeeking.h"
#include "Loading/ZoneLoader.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <string>

using namespace std::string_literals;
namespace fs = std::filesystem;

std::expected<std::unique_ptr<Zone>, std::string> ZoneLoading::LoadZone(const std::string& path,
                                                                        std::optional<std::unique_ptr<ProgressCallback>> progressCallback)
{
    auto zoneName = fs::path(path).filename().replace_extension().string();
    std::ifstream file(path, std::fstream::in | std::fstream::binary);

    if (!file.is_open())
        return std::unexpected(std::format("Could not open file '{}'.", path));

    ZoneDataPeeking dataPeeking(file);
    std::unique_ptr<ZoneLoader> zoneLoader;
    for (auto game = 0u; game < static_cast<unsigned>(GameId::COUNT); game++)
    {
        const auto* factory = IZoneLoaderFactory::GetZoneLoaderFactoryForGame(static_cast<GameId>(game));
        if (factory->InspectZoneHeader(dataPeeking))
        {
            zoneLoader = factory->CreateLoaderForHeader(dataPeeking, zoneName, std::move(progressCallback));
            break;
        }
    }

    if (!zoneLoader)
        return std::unexpected(std::format("Could not create factory for zone '{}'.", zoneName));

    // Revert to beginning to make sure no peeking has advanced the read position.
    // (As different peeking logic may advance by different amounts of bytes).
    file.seekg(0, std::ios::beg);

    auto loadedZone = zoneLoader->LoadZone(file);

    file.close();

    if (!loadedZone)
        return std::unexpected("Loading zone failed."s);

    return std::move(loadedZone);
}
