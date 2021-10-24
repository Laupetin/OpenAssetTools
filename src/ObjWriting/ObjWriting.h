#pragma once

#include <vector>

#include "Dumping/AssetDumpingContext.h"
#include "Zone/ZoneTypes.h"

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
        std::vector<bool> AssetTypesToHandleBitfield;

        ImageOutputFormat_e ImageOutputFormat = ImageOutputFormat_e::DDS;
        ModelOutputFormat_e ModelOutputFormat = ModelOutputFormat_e::XMODEL_EXPORT;
        bool MenuLegacyMode = false;

    } Configuration;

    static bool DumpZone(AssetDumpingContext& context);
    static bool ShouldHandleAssetType(asset_type_t assetType);
};
