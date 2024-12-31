#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace T6
{
    std::unique_ptr<AssetCreator<AssetQdb>> CreateQdbLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace T6
