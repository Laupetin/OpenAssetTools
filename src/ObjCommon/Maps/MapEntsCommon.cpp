#include "MapEntsCommon.h"

#include <format>

namespace map_ents
{
    std::string GetEntsFileNameForAssetName(const std::string& assetName)
    {
        return std::format("{}.ents", assetName);
    }
} // namespace map_ents
