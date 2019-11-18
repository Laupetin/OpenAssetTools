#include "ZoneLoading.h"
#include "Utils/PathUtils.h"
#include "Dumping/IZoneDumper.h"

#include "Game/T6/ZoneLoaderFactoryT6.h"
#include "Game/T6/ZoneDumperT6.h"

IZoneLoaderFactory* zoneLoaderFactories[]
{
    new ZoneLoaderFactoryT6()
};

IZoneDumper* zoneDumper[]
{
    new ZoneDumperT6()
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

bool ZoneLoading::DumpZone(Zone* zone, const std::string& basePath)
{
    for(auto dumper : zoneDumper)
    {
        if(dumper->CanHandleZone(zone))
        {
            if(dumper->DumpZone(zone, basePath))
            {
                return true;
            }

            printf("Dumper for zone '%s' failed!\n", zone->m_name.c_str());
            return false;
        }
    }

    return false;
}

bool ZoneLoading::WriteZoneDefinition(Zone* zone, FileAPI::File* file, bool minimalistic)
{
    return file->Printf("// %s", "Insert zone definition here") > 0;
}