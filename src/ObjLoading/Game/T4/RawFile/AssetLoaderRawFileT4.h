#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T4/T4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace raw_file
{
    std::unique_ptr<AssetCreator<T4::AssetRawFile>> CreateLoaderT4(MemoryManager& memory, ISearchPath& searchPath);
} // namespace raw_file
