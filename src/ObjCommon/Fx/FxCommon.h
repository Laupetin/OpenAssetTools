#pragma once

#include <string>

namespace fx
{
    struct RgbaColor
    {
        float r;
        float g;
        float b;
        float a;
    };

    std::string GetFileNameForAssetName(const std::string& assetName);
}
