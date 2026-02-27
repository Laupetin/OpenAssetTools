#pragma once

#include "Asset/IAssetCreator.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace techset
{
    std::unique_ptr<ISubAssetCreator> CreateVertexDeclCompilerT6(MemoryManager& memory);
}
