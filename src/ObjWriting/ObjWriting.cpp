#include "ObjWriting.h"
#include "Dumping/IZoneDumper.h"
#include "Game/T6/ZoneDumperT6.h"

ObjWriting::Configuration_t ObjWriting::Configuration;

const IZoneDumper* const ZONE_DUMPER[]
{
    new ZoneDumperT6()
};

bool ObjWriting::DumpZone(Zone* zone, const std::string& basePath)
{
    for (auto dumper : ZONE_DUMPER)
    {
        if (dumper->CanHandleZone(zone))
        {
            if (dumper->DumpZone(zone, basePath))
            {
                return true;
            }

            printf("Dumper for zone '%s' failed!\n", zone->m_name.c_str());
            return false;
        }
    }

    return false;
}

bool ObjWriting::WriteZoneDefinition(Zone* zone, FileAPI::File* file)
{
    return file->Printf("// %s", "Insert zone definition here") > 0;
}