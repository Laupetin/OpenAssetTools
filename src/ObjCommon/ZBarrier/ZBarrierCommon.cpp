#include "ZBarrierCommon.h"

#include <format>

namespace z_barrier
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("zbarrier/{}", assetName);
    }
} // namespace z_barrier
