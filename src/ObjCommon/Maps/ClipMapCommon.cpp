#include "ClipMapCommon.h"

#include <format>

namespace clip_map
{
    std::string GetJsonFileNameForAssetName(const std::string& assetName)
    {
        return std::format("{}.clipmap.json", assetName);
    }

    std::string GetVertexFileNameForAssetName(const std::string& assetName)
    {
        return std::format("{}.clipmap_verts.bin", assetName);
    }

    std::string GetTriangleFileNameForAssetName(const std::string& assetName)
    {
        return std::format("{}.clipmap_tris.bin", assetName);
    }

    std::string GetEdgeWalkableFileNameForAssetName(const std::string& assetName)
    {
        return std::format("{}.clipmap_edgewalkable.bin", assetName);
    }
} // namespace clip_map
