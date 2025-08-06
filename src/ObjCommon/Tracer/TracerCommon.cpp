#include "TracerCommon.h"

#include <format>

namespace tracer
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("tracer/{}", assetName);
    }
} // namespace tracer
