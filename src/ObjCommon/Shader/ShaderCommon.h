#pragma once

#include <string>

namespace shader
{
    std::string GetFileNameForPixelShaderAssetName(const std::string& assetName);
    std::string GetFileNameForVertexShaderAssetName(const std::string& assetName);
} // namespace shader
