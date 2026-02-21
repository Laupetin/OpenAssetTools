#pragma once

#include "Asset/IAssetCreator.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace techset
{
    std::unique_ptr<ISubAssetCreator> CreateTechniqueCompilerT6(MemoryManager& memory, ISearchPath& searchPath);
}
