#include "ObjWriting.h"
#include "Dumping/IZoneDumper.h"
#include "Game/IW3/ZoneDumperIW3.h"
#include "Game/IW4/ZoneDumperIW4.h"
#include "Game/IW5/ZoneDumperIW5.h"
#include "Game/T5/ZoneDumperT5.h"
#include "Game/T6/ZoneDumperT6.h"

ObjWriting::Configuration_t ObjWriting::Configuration;

const IZoneDumper* const ZONE_DUMPER[]
{
    new IW3::ZoneDumper(),
    new IW4::ZoneDumper(),
    new IW5::ZoneDumper(),
    new T5::ZoneDumper(),
    new T6::ZoneDumper()
};

bool ObjWriting::DumpZone(AssetDumpingContext& context)
{
    for (const auto* dumper : ZONE_DUMPER)
    {
        if (dumper->CanHandleZone(context))
        {
            if (dumper->DumpZone(context))
            {
                return true;
            }

            printf("Dumper for zone '%s' failed!\n", context.m_zone->m_name.c_str());
            return false;
        }
    }

    return false;
}

bool ObjWriting::ShouldHandleAssetType(const asset_type_t assetType)
{
    if (assetType < 0)
        return false;
    if (static_cast<size_t>(assetType) >= Configuration.AssetTypesToHandleBitfield.size())
        return true;

    return Configuration.AssetTypesToHandleBitfield[assetType];
}
