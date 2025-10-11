#include "ShaderCommon.h"

#include <format>

namespace shader
{
    std::string GetFileNameForPixelShaderAssetName(const std::string& assetName)
    {
        return std::format("techniquesets/shader_bin/ps_{}.cso", assetName);
    }

    std::string GetFileNameForVertexShaderAssetName(const std::string& assetName)
    {
        return std::format("techniquesets/shader_bin/vs_{}.cso", assetName);
    }
} // namespace shader
