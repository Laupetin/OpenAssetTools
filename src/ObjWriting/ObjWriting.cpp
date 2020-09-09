#include "ObjWriting.h"
#include "Dumping/IZoneDumper.h"
#include "Game/IW4/ZoneDumperIW4.h"
#include "Game/T6/ZoneDumperT6.h"

ObjWriting::Configuration_t ObjWriting::Configuration;

const IZoneDumper* const ZONE_DUMPER[]
{
    new IW4::ZoneDumper(),
    new T6::ZoneDumper()
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