#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace T6::phys_constraints
{
    std::unique_ptr<AssetCreator<AssetPhysConstraints>> CreateRawLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace T6::phys_constraints
