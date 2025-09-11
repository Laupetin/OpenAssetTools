#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Zone/ZoneTypes.h"

#include <vector>

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
            XMODEL_BIN,
            OBJ,
            GLTF,
            GLB
        };

        std::vector<bool> AssetTypesToHandleBitfield;

        ImageOutputFormat_e ImageOutputFormat = ImageOutputFormat_e::DDS;
        ModelOutputFormat_e ModelOutputFormat = ModelOutputFormat_e::GLB;
        bool MenuLegacyMode = false;

    } Configuration;

    static bool ShouldHandleAssetType(asset_type_t assetType);
};
