#include "ZoneLoading.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "Game/IW4/ZoneLoaderFactoryIW4.h"
#include "Game/T6/ZoneLoaderFactoryT6.h"
#include "Utils/ObjFileStream.h"

namespace fs = std::filesystem;

IZoneLoaderFactory* ZoneLoaderFactories[]
{
    new IW4::ZoneLoaderFactory(),
    new T6::ZoneLoaderFactory()
};

std::unique_ptr<Zone> ZoneLoading::LoadZone(const std::string& path)
{
    auto zoneName = fs::path(path).filename().replace_extension("").string();
    std::ifstream file(path, std::fstream::in | std::fstream::binary);

    if(!file.is_open())
    {
        printf("Could not open file '%s'.\n", path.c_str());
        return nullptr;
    }

    ZoneHeader header{};
    file.read(reinterpret_cast<char*>(&header), sizeof header);
    if(file.gcount() != sizeof header)
    {
        std::cout << "Failed to read zone header from file '" << path << "'.\n";
        return nullptr;
    }

    ZoneLoader* zoneLoader = nullptr;
    for(auto* factory : ZoneLoaderFactories)
    {
        zoneLoader = factory->CreateLoaderForHeader(header, zoneName);

        if(zoneLoader != nullptr)
            break;
    }

    if(zoneLoader == nullptr)
    {
        printf("Could not create factory for zone '%s'.\n", zoneName.c_str());
        return nullptr;
    }

    auto loadedZone = zoneLoader->LoadZone(file);
    delete zoneLoader;

    file.close();
    return std::move(loadedZone);
}