#include "ZoneLoading.h"

#include "Loading/IZoneLoaderFactory.h"
#include "Loading/ZoneLoader.h"
#include "Utils/ObjFileStream.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

result::Expected<std::unique_ptr<Zone>, std::string> ZoneLoading::LoadZone(const std::string& path)
{
    auto zoneName = fs::path(path).filename().replace_extension().string();
    std::ifstream file(path, std::fstream::in | std::fstream::binary);

    if (!file.is_open())
        return result::Unexpected(std::format("Could not open file '{}'.", path));

    ZoneHeader header{};
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    if (file.gcount() != sizeof(header))
        return result::Unexpected(std::format("Failed to read zone header from file '{}'.", path));

    std::unique_ptr<ZoneLoader> zoneLoader;
    for (auto game = 0u; game < static_cast<unsigned>(GameId::COUNT); game++)
    {
        const auto* factory = IZoneLoaderFactory::GetZoneLoaderFactoryForGame(static_cast<GameId>(game));
        zoneLoader = factory->CreateLoaderForHeader(header, zoneName);

        if (zoneLoader)
            break;
    }

    if (!zoneLoader)
        return result::Unexpected(std::format("Could not create factory for zone '{}'.", zoneName));

    auto loadedZone = zoneLoader->LoadZone(file);

    file.close();
    return std::move(loadedZone);
}
