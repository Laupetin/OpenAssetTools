#include "ObjWriting.h"
#include "Dumping/IZoneDumper.h"
#include "Game/T6/ZoneDumperT6.h"

IZoneDumper* zoneDumper[]
{
    new ZoneDumperT6()
};

bool ObjWriting::DumpZone(Zone* zone, const std::string& basePath)
{
    for (auto dumper : zoneDumper)
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

bool ObjWriting::WriteZoneDefinition(Zone* zone, FileAPI::File* file, bool minimalistic)
{
    return file->Printf("// %s", "Insert zone definition here") > 0;
}