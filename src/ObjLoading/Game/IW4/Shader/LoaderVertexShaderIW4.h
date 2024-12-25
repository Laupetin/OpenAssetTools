#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace IW4
{
    [[nodiscard]] std::string GetVertexShaderFileName(const std::string& vertexShaderAssetName);

    std::unique_ptr<AssetCreator<AssetVertexShader>> CreateVertexShaderLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace IW4
