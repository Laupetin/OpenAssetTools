#pragma once

#include "Dumping/AssetDumpingContext.h"

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

    } Configuration;

    static bool DumpZone(AssetDumpingContext& context);
};
