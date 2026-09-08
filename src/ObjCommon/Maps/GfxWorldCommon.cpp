#include "GfxWorldCommon.h"

#include <format>

namespace gfx_world
{
    std::string GetJsonFileNameForAssetName(const std::string& assetName)
    {
        return std::format("{}.gfxworld.json", assetName);
    }

    std::string GetVertexFileNameForAssetName(const std::string& assetName)
    {
        return std::format("{}.gfxworld_vertices.bin", assetName);
    }

    std::string GetIndexFileNameForAssetName(const std::string& assetName)
    {
        return std::format("{}.gfxworld_indices.bin", assetName);
    }
} // namespace gfx_world
