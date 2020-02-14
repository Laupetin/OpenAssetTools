#pragma once

#include "Zone/Zone.h"
#include "Utils/FileAPI.h"
#include <string>

class ObjWriting
{
public:
    static class Configuration_t
    {
    public:
        enum class ImageOutputFormat_e
        {
            DDS,
            IWI
        };

        bool Verbose = false;
        ImageOutputFormat_e ImageOutputFormat = ImageOutputFormat_e::DDS;
        bool MinimalZoneFileOutput = false;

    } Configuration;

    static bool DumpZone(Zone* zone, const std::string& basePath);
    static bool WriteZoneDefinition(Zone* zone, FileAPI::File* file);
};
