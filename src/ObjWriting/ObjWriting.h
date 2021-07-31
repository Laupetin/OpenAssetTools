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

        enum class ModelOutputFormat_e
        {
            XMODEL_EXPORT,
            OBJ
        };

        bool Verbose = false;
        ImageOutputFormat_e ImageOutputFormat = ImageOutputFormat_e::DDS;
        ModelOutputFormat_e ModelOutputFormat = ModelOutputFormat_e::XMODEL_EXPORT;

    } Configuration;

    static bool DumpZone(AssetDumpingContext& context);
};
