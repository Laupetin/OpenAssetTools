#include "ZoneLoading.h"
#include "Game/T6/ZoneLoaderFactoryT6.h"
#include "Utils/PathUtils.h"

IZoneLoaderFactory* zoneLoaderFactories[]
{
    new ZoneLoaderFactoryT6()
};

Zone* ZoneLoading::LoadZone(const std::string& path)
{
    std::string zoneName = utils::Path::GetFilenameWithoutExtension(path);
    FileAPI::File file = FileAPI::Open(path, FileAPI::Mode::MODE_READ);

    if(!file.IsOpen())
    {
        printf("Could not open file '%s'.\n", path.c_str());
        return nullptr;
    }

    ZoneHeader header{};
    file.Read(&header, sizeof(ZoneHeader), 1);

    ZoneLoader* zoneLoader = nullptr;
    for(auto factory : zoneLoaderFactories)
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

    Zone* loadedZone = zoneLoader->LoadZone(&file);

    file.Close();
    return loadedZone;
}