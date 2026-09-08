#pragma once

#include <string>

namespace gfx_world
{
    std::string GetJsonFileNameForAssetName(const std::string& assetName);
    std::string GetVertexFileNameForAssetName(const std::string& assetName);
    std::string GetIndexFileNameForAssetName(const std::string& assetName);
} // namespace gfx_world
