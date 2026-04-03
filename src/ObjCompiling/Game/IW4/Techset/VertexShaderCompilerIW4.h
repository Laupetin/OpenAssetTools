#pragma once

#include "Asset/IAssetCreator.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace techset
{
    std::unique_ptr<IAssetCreator> CreateVertexShaderCompilerIW4(MemoryManager& memory, ISearchPath& searchPath);
}
