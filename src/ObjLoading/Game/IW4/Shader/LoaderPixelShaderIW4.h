#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace IW4
{
    [[nodiscard]] std::string GetPixelShaderFileName(const std::string& pixelShaderAssetName);

    std::unique_ptr<AssetCreator<AssetPixelShader>> CreatePixelShaderLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace IW4
