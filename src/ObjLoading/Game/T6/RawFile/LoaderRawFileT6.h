#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace raw_file
{
    std::unique_ptr<AssetCreator<T6::AssetRawFile>> CreateLoaderT6(MemoryManager& memory, ISearchPath& searchPath);
} // namespace raw_file
