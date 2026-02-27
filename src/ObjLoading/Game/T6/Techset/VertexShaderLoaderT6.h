#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace techset
{
    std::unique_ptr<SubAssetCreator<T6::SubAssetVertexShader>> CreateVertexShaderLoaderT6(MemoryManager& memory, ISearchPath& searchPath);
} // namespace techset
