#pragma once

#include <string>

namespace clip_map
{
    std::string GetJsonFileNameForAssetName(const std::string& assetName);
    std::string GetVertexFileNameForAssetName(const std::string& assetName);
    std::string GetTriangleFileNameForAssetName(const std::string& assetName);
    std::string GetEdgeWalkableFileNameForAssetName(const std::string& assetName);
} // namespace clip_map
